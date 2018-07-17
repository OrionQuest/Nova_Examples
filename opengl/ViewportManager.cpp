#include "ViewportManager.h"
#include "ApplicationFactory.h"
#include "IOEvent.h"
#include "Camera.h"
#include "CameraControlInterface.h"
#include "OpenGLState.h"

#include "StaticCameraControl.h"
#include "TrackballCameraControl.h"
#include "OrbitCameraControl.h"
#include "PlanarCameraControl.h"

#include <stdexcept>

#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>

Nova::ViewportManager::ViewportManager( ApplicationFactory &app ) :
    _app(app), _geometry(0,0), _activeViewport(0), _recentlyInteractedViewport(0), axis_mode(AXIS_CORNER),
    sliceModeConfigured(false), _debug_frustrum(false), DEFAULT_CLIP_NEAR(0.1f), DEFAULT_CLIP_FAR(10000.0f),
    _currentConfiguration(VM_NOT_CONFIGURED)
{

    // Set up some built in control schemes
    _controllerFactories.insert( std::make_pair( "STATIC-CAMERA", std::move( std::unique_ptr<CameraFactory>( new SimpleCameraFactory<StaticCameraControl> ))));
    _controllerFactories.insert( std::make_pair( "ORBIT-CAMERA", std::move( std::unique_ptr<CameraFactory>( new SimpleCameraFactory<OrbitCameraControl> ))));
    _controllerFactories.insert( std::make_pair( "TRACKBALL-CAMERA", std::move( std::unique_ptr<CameraFactory>( new SimpleCameraFactory<TrackballCameraControl> ))));
    _controllerFactories.insert( std::make_pair( "PLANAR-CAMERA", std::move( std::unique_ptr<CameraFactory>( new SimpleCameraFactory<PlanarCameraControl> ))));
    
}

Nova::ViewportManager::~ViewportManager() {

}

unsigned int
Nova::ViewportManager::NumViewports(){
    return _viewports.size();
}


unsigned int
Nova::ViewportManager::CurrentViewport() {
    return _activeViewport;
}


void
Nova::ViewportManager::SetWindowGeometry(int width, int height) {
    // Update the global window viewport
    _global_viewport.x = 0;
    _global_viewport.y = 0;
    _global_viewport.width = width;
    _global_viewport.height = height;

    // Now update all viewports based on the active configuration
    switch( _currentConfiguration ){
    case VM_SINGLE_VIEWPORT:
        {
            Viewport& viewport = _viewports[0]; // This mode only has one viewport
            // ... and its pretty easy to do so.
            viewport.x = _global_viewport.x;
            viewport.y = _global_viewport.y;
            viewport.width  = _global_viewport.width;
            viewport.height = _global_viewport.height;
           
        }
        break;
    case VM_DUAL_VIEWPORT:
        {
            Viewport& viewportA = _viewports[0]; 
            viewportA.x = _global_viewport.x;
            viewportA.y = _global_viewport.y;
            viewportA.width  = _global_viewport.width/2;
            viewportA.height = _global_viewport.height;

            Viewport& viewportB = _viewports[1]; 
            viewportB.x = _global_viewport.x + _global_viewport.width/2;
            viewportB.y = _global_viewport.y;
            viewportB.width  = _global_viewport.width/2;
            viewportB.height = _global_viewport.height;

        }
        break;
    }

    for( int v = 0; v < this->_viewports.size(); v++ )
        // Next, we need to manually update each controller based on its viewport's
        // unique geometry
        UpdateViewportGeometry(v);   
}

void
Nova::ViewportManager::UpdateViewportGeometry( unsigned int v ){
    Viewport& viewport = _viewports.at(v);
    Nova::IOEvent event(IOEvent::DRAW);
    event.draw_data->x = viewport.x;
    event.draw_data->y = viewport.y;
    event.draw_data->width = viewport.width;
    event.draw_data->height = viewport.height;
    if( viewport.controller )
        viewport.controller->Redraw( event );                
}
    
void
Nova::ViewportManager::SetGlobalViewport(){
    _activeViewport = GLOBAL_VIEWPORT_ID;    
}

void
Nova::ViewportManager::SetViewport( unsigned int viewport ){
    if( viewport >= _viewports.size())
        throw std::out_of_range( "Unable to switch active viewport. Reqested viewport out of range." );
    _activeViewport = viewport;
}


void 
Nova::ViewportManager::ConfigureViewports( ViewportConfiguration vc ){
    // Setup viewports based on the specific configuration layout.

    int numOldViewports = 0;
    int numNewViewports = 0;

    switch( _currentConfiguration ){
    case VM_SINGLE_VIEWPORT:
        numOldViewports=1;
        break;
    case VM_DUAL_VIEWPORT:
        numOldViewports=2;
        break;
    default:
        numOldViewports=0;
        break;       
    }
    switch( vc ){
    case VM_SINGLE_VIEWPORT:
        numNewViewports=1;
        break;
    case VM_DUAL_VIEWPORT:
        numNewViewports=2;
        break;
    default:
        numNewViewports=0;
        break;       
    }
          
    glm::vec4 scene_bounding_sphere = _app.GetScene().BoundingSphere();
    glm::vec3 initial_position = glm::vec3(scene_bounding_sphere) + glm::vec3(0,0,scene_bounding_sphere.w);
    glm::vec3 initial_lookat = glm::vec3(scene_bounding_sphere);

    for( int addedViewports = 0; addedViewports < std::max(0, numNewViewports-numOldViewports); addedViewports++ ){
        Viewport viewport;
        viewport.clipNear = DEFAULT_CLIP_NEAR;
        viewport.clipFar = DEFAULT_CLIP_FAR;
        viewport.camera = std::unique_ptr<Camera>( new Camera() );
        viewport.camera->Set_Mode(FREE);
        viewport.camera->Set_FOV(45);
        viewport.camera->Set_Position(initial_position);
        viewport.camera->Set_Look_At(initial_lookat);
        viewport.render_style = RS_SOLID;
        _viewports.push_back( std::move(viewport) );
        
        Set_Controller("ORBIT-CAMERA", _viewports.size()-1);        
    }
    for( int removedViewports = 0; removedViewports < std::max(0, numOldViewports-numNewViewports); removedViewports++ ){
        _viewports.pop_back();  // We can do this safely as the unique_ptr's will clean up the camera and controller
    }
    _currentConfiguration = vc;
    _activeViewport = 0;
    _recentlyInteractedViewport = 0;
    _focusedViewport = 0;

    SetWindowGeometry( _global_viewport.width, _global_viewport.height );
}

void
Nova::ViewportManager::InitializeChrome(){
    
    // Initialize buffers for the chrome
    glGenVertexArrays(1, &this->chromeVAO);
    glGenBuffers(1, &this->chromeVBO);
    glBindVertexArray(this->chromeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->chromeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Initialize buffers for the axis displays
    glGenVertexArrays(1, &this->axisVAO);
    glGenBuffers(1, &this->axisVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Initialize buffers for the frustrum displays
    std::array<GLuint, 24> frustrum_indices;
    frustrum_indices[0] = 0; frustrum_indices[1] = 1;
    frustrum_indices[2] = 3; frustrum_indices[3] = 2;
    frustrum_indices[4] = 0; frustrum_indices[5] = 100;
    frustrum_indices[6] = 4; frustrum_indices[7] = 5;
    frustrum_indices[8] = 7; frustrum_indices[9] = 6;
    frustrum_indices[10] = 4; frustrum_indices[11] = 100;
    frustrum_indices[12] = 0; frustrum_indices[13] = 4;
    frustrum_indices[14] = 100;
    frustrum_indices[15] = 1; frustrum_indices[16] = 5;
    frustrum_indices[17] = 100;
    frustrum_indices[18] = 2; frustrum_indices[19] = 6;
    frustrum_indices[20] = 100;
    frustrum_indices[21] = 3; frustrum_indices[22] = 7;
    frustrum_indices[23] = 100;
    glGenVertexArrays(1, &this->frustrumVAO);
    glGenBuffers(1, &this->frustrumVBO);
    glGenBuffers(1, &this->frustrumEBO);
    glBindVertexArray(this->frustrumVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->frustrumVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8 * 4, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->frustrumEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24*sizeof(GLuint), &frustrum_indices[0], GL_STATIC_DRAW); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}

void
Nova::ViewportManager::DrawFrame(){
    if( _activeViewport != GLOBAL_VIEWPORT_ID )
        return;

    // Perhaps there is a way to dynamically determine the correct
    // dividing lines between panes, but for now we will hardcode them
    glm::vec4 geometry = GetViewport();
    //glViewport(geometry.x,geometry.y,geometry.z,geometry.w);
    GLfloat vertices[6][4] = {
        { 0,     geometry.w,   0.0, 1.0 },
        { geometry.z, 0,       1.0, 0.0 },
        { 0,     0,       0.0, 0.0 },
        
        { 0,     geometry.w,   0.0, 1.0 },
        { geometry.z, geometry.w,   1.0, 1.0 },
        { geometry.z, 0,       1.0, 0.0 }
    };
    

    {
        auto _shader = _app.GetShaderManager().GetShader( "ViewportChromeShader" );
        _shader->SetMatrix4("projection", glm::ortho(0.0f,
                                                     static_cast<GLfloat>(geometry.z),
                                                     static_cast<GLfloat>(geometry.w),
                                                     0.0f));
        _shader->SetInteger("panes", _viewports.size() );
        _shader->SetFloat("thickness", 1.0f ); // Two pixels thick
        _shader->SetFloat("width", geometry.z );
        _shader->SetFloat("height", geometry.w );
        
        glBindVertexArray(this->chromeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->chromeVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);    
        glBindVertexArray(0);    
    }
};

void
Nova::ViewportManager::DrawFrustrums(){
    if( !_debug_frustrum )
        return;

    std::array< glm::vec3, 8 > frustrum_verts;
    glm::mat4 viewport_projection, viewport_view, viewport_model;
    viewport_view = _app.GetWorld().Get_ViewMatrix();
    viewport_model = _app.GetWorld().Get_ModelMatrix();
    viewport_projection = _app.GetWorld().Get_ProjectionMatrix();

    auto _shader = _app.GetShaderManager().GetShader( "BasicColored" );
    _shader->SetMatrix4("projection", viewport_projection);
    _shader->SetMatrix4("view", viewport_view);
    _shader->SetMatrix4("model", viewport_model);

    for( int v = 0; v < _viewports.size(); v++ ){       
        if( v == _activeViewport)
            continue;

        GLStateEnable enablePrimRestart(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(100); // We use this to identify restart indices

        Viewport& viewport = _viewports.at(v);        
        glm::mat4 projection, view;
        projection = viewport.camera->Get_ProjectionMatrix();
        view = viewport.camera->Get_ViewMatrix();
        
        frustrum_verts[0] = glm::vec3(-1,-1,-1);
        frustrum_verts[1] = glm::vec3(-1,-1, 1);
        frustrum_verts[2] = glm::vec3(-1, 1,-1);
        frustrum_verts[3] = glm::vec3(-1, 1, 1);
        frustrum_verts[4] = glm::vec3( 1,-1,-1);
        frustrum_verts[5] = glm::vec3( 1,-1, 1);
        frustrum_verts[6] = glm::vec3( 1, 1,-1);
        frustrum_verts[7] = glm::vec3( 1, 1, 1);
        for( int i = 0; i < 8; i++){
            glm::vec4 modified_vert = glm::inverse( projection * view ) * glm::vec4(frustrum_verts[i],1.0f);
            frustrum_verts[i] = glm::vec3( modified_vert * (float)( 1.0 / modified_vert.w )) ;
        }

        _shader->SetVector3f("basecolor", glm::vec3(0.8,0.8,0.8) );
        glBindVertexArray(this->frustrumVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->frustrumVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*8, &frustrum_verts[0]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_LINE_STRIP, 24, GL_UNSIGNED_INT, 0);    
        glBindVertexArray(0);


        frustrum_verts[0] = glm::vec3(-1,-1,-1);
        frustrum_verts[1] = glm::vec3(-1,-1, 1);
        frustrum_verts[2] = glm::vec3(-1, 1,-1);
        frustrum_verts[3] = glm::vec3(-1, 1, 1);
        frustrum_verts[4] = glm::vec3( 1,-1,-1);
        frustrum_verts[5] = glm::vec3( 1,-1, 1);
        frustrum_verts[6] = glm::vec3( 1, 1,-1);
        frustrum_verts[7] = glm::vec3( 1, 1, 1);
        for( int i = 0; i < 8; i++){
            glm::vec4 modified_vert = glm::inverse( view ) * glm::vec4(frustrum_verts[i],1.0f);
            frustrum_verts[i] = glm::vec3( modified_vert * (float)( 1.0 / modified_vert.w )) ;
        }

        _shader->SetVector3f("basecolor", glm::vec3(0.8,0.1,0.8) );
        glBindVertexArray(this->frustrumVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->frustrumVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*8, &frustrum_verts[0]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_LINE_STRIP, 24, GL_UNSIGNED_INT, 0);    
        glBindVertexArray(0);

        glDisable(GL_PRIMITIVE_RESTART);
    }

}


void Nova::ViewportManager::DrawAxis(){
    if( axis_mode == AXIS_DISABLE )
        return;
    if( _activeViewport == GLOBAL_VIEWPORT_ID )
        return;

    Viewport& viewport = _viewports.at( _activeViewport );

    auto _shader = _app.GetShaderManager().GetShader( "ViewportAxisShader" );
    glm::mat4 projection,view,model;
    model = viewport.camera->Get_ModelMatrix();
    view = viewport.camera->Get_ViewMatrix();
    glm::mat3 rot_part( view );   
    glm::mat4 mReflectionX = glm::mat4
        (
         -1.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0,
         0.0, 0.0, 1.0, 0.0,
         0.0, 0.0, 0.0, 1.0
         );
    
    view =  glm::rotate( mReflectionX * glm::mat4(rot_part), glm::pi<float>(), glm::vec3( 1.0, 0, 0) ) * mReflectionX;
    if( axis_mode == AXIS_CENTER ){
        projection = glm::ortho(-1.0f * ((float)(viewport.width) / viewport.height),
                                  1.0f * ((float)(viewport.width) / viewport.height), 
                                  1.0f ,
                                  -1.0f,
                                  -1.0f,
                                  1.0f);        
        glViewport( viewport.x, viewport.y, viewport.width, viewport.height );
    }
    if( axis_mode == AXIS_CORNER ){
        projection = glm::ortho(-1.0f,
                                  1.0f, 
                                  1.0f ,
                                  -1.0f,
                                  -1.0f,
                                  1.0f);        
        glViewport( viewport.x, viewport.y, 50, 50 );
    }
    
    _shader->SetMatrix4("projection",projection);
    _shader->SetMatrix4("view",view);
    _shader->SetMatrix4("model",model);
    
    glBindVertexArray(this->axisVAO);
    
    GLfloat vertices[1][4] = {
        { 0,  0,  0, 1.0 }
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, this->axisVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);  
}

std::vector< std::string >
Nova::ViewportManager::AvailableControllers(){
    // TODO: return all the registered controller factories by name

    return std::vector<std::string>();
}

void
Nova::ViewportManager::ResetAll(){
    // Reset all camera controllers for all viewports
    for( auto&& viewport : _viewports ){
        if( viewport.controller )
            viewport.controller->Reset();
        
        // Also reset clipping planes, if the user has messed with them..
        viewport.clipNear = DEFAULT_CLIP_NEAR;
        viewport.clipFar = DEFAULT_CLIP_FAR;
    }
}

void
Nova::ViewportManager::Update(){
    //  Run the update routine for the currently active viewport's camera controller
    if( _activeViewport == GLOBAL_VIEWPORT_ID ){
        glViewport(_global_viewport.x,_global_viewport.y,_global_viewport.width,_global_viewport.height);
    }
    else
    {
        Viewport& viewport = _viewports.at( _activeViewport );
        viewport.camera->Set_Clipping( viewport.clipNear, viewport.clipFar);
        viewport.camera->Set_Viewport( viewport.x, viewport.y, viewport.width, viewport.height );
        viewport.camera->Update();
        glViewport( viewport.x, viewport.y, viewport.width, viewport.height );
    }
}

void
Nova::ViewportManager::Set_ClipBounds( float near, float far, unsigned int v ){
    // Update active viewport's clip bounds
    Viewport& viewport = _viewports.at( v );
    viewport.clipNear = near;
    viewport.clipFar = far;    
}

void
Nova::ViewportManager::Set_Controller( std::string controller, unsigned int v ){
    // Replace the active viewport's controller with one from factory 'controller'
    auto res = _controllerFactories.find( controller );
    if( res == _controllerFactories.end() )
        throw std::runtime_error( "Cannot replace viewport controller. No such controller type is currently registered");

    Viewport& viewport = _viewports.at( v );
    if( viewport.controller )
        viewport.controller->Reset();
    viewport.controller.reset();
    viewport.controller = res->second->BuildCameraController( *(viewport.camera.get()) ); // Is this the best way???
    UpdateViewportGeometry(v);
    viewport.controller->Reset();
}

void 
Nova::ViewportManager::Configure_SliceMode( glm::vec3 start, float dx, glm::vec3 max_slice )
{
    referenceSliceInfo.axis = SLICE_X;
    referenceSliceInfo.slice_start = start;
    referenceSliceInfo.max_slice = max_slice;
    referenceSliceInfo.slice_dx = dx;
    referenceSliceInfo.slice_inc = 0;
    sliceModeConfigured = true;
}

void
Nova::ViewportManager::Toggle_SliceMode( unsigned int v ){
    Viewport& viewport = _viewports.at( v );
    if( !sliceModeConfigured ){
        viewport.render_style &= ~RS_SLICE;
        return;
    }

    viewport.render_style ^= RS_SLICE;
    if( viewport.render_style & RS_SLICE ){
        viewport.slice_info = referenceSliceInfo;
    }
}

void
Nova::ViewportManager::Change_Slice( unsigned int v, int slice_offset ){
    Viewport& viewport = _viewports.at( v );
    if( viewport.render_style & RS_SLICE ){
        switch( viewport.slice_info.axis ){
        case SLICE_X:
            viewport.slice_info.slice_inc = std::max( 0, std::min((int)viewport.slice_info.max_slice.x, viewport.slice_info.slice_inc + slice_offset) );
            break;
        case SLICE_Y:
            viewport.slice_info.slice_inc = std::max( 0, std::min((int)viewport.slice_info.max_slice.y, viewport.slice_info.slice_inc + slice_offset) );
            break;
        case SLICE_Z:
            viewport.slice_info.slice_inc = std::max( 0, std::min((int)viewport.slice_info.max_slice.z, viewport.slice_info.slice_inc + slice_offset) );
            break;
        }
    }
}

void Nova::ViewportManager::Change_SliceAxis( unsigned int v ){
    Viewport& viewport = _viewports.at( v );
    viewport.slice_info.axis = (SliceAxis)( (viewport.slice_info.axis + 1) % MAX_AXIS );
    Change_Slice(v, 0 ); // Trigger a slice change in case the new axis has different bounds.
}

bool 
Nova::ViewportManager::IsPrimaryViewport() const {
    return _activeViewport == 0;
}

unsigned int
Nova::ViewportManager::GlobalViewportId() const {
    return GLOBAL_VIEWPORT_ID;
}

unsigned int
Nova::ViewportManager::ActiveViewportId() const {
    return _activeViewport;
}

unsigned int
Nova::ViewportManager::InteractionViewportId() const {
    return _recentlyInteractedViewport;
}

unsigned int
Nova::ViewportManager::FocusedViewportId() const {
    return _focusedViewport;
}


uint32_t
Nova::ViewportManager::RenderStyle(uint32_t mask) const {
    return RenderStyle(mask, ActiveViewportId() );
}

uint32_t
Nova::ViewportManager::RenderStyle(uint32_t mask, unsigned int v) const {
    if( v == GLOBAL_VIEWPORT_ID )
        return _global_viewport.render_style & mask;
    
    const Viewport& viewport = _viewports.at( v );
    return (viewport.render_style & mask);
}

glm::vec3 
Nova::ViewportManager::GetCameraPosition() const {
    if( _activeViewport == GLOBAL_VIEWPORT_ID )
        return glm::vec3();

    const Viewport& viewport = _viewports.at( _activeViewport );
    return viewport.camera->Get_Position();
}

std::array<glm::vec4, 2>
Nova::ViewportManager::GetSlicePlanes() const {
    return GetSlicePlanes( ActiveViewportId() );
}

std::array<glm::vec4, 2>
Nova::ViewportManager::GetSlicePlanes(unsigned int v) const {
    std::array<glm::vec4, 2> output;

    if( v == GLOBAL_VIEWPORT_ID )
        return output;

    const Viewport& viewport = _viewports.at( v );
    
    float slice_distance_min = (-1 * viewport.slice_info.slice_inc * viewport.slice_info.slice_dx) + (viewport.slice_info.slice_dx/2.0f);
    float slice_distance_max = (viewport.slice_info.slice_inc * viewport.slice_info.slice_dx) + (viewport.slice_info.slice_dx/2.0f);

    switch( viewport.slice_info.axis ){
    case SLICE_X:
        output[0] = glm::vec4(1,0,0,0);
        output[1] = glm::vec4(-1,0,0,0);
        output[0].w = slice_distance_min - viewport.slice_info.slice_start.x;
        output[1].w = slice_distance_max + viewport.slice_info.slice_start.x;
        break;
    case SLICE_Y:
        output[0] = glm::vec4(0,1,0,0);
        output[1] = glm::vec4(0,-1,0,0);
        output[0].w = slice_distance_min - viewport.slice_info.slice_start.y;
        output[1].w = slice_distance_max + viewport.slice_info.slice_start.y;
        break;
    case SLICE_Z:
        output[0] = glm::vec4(0,0,1,0);
        output[1] = glm::vec4(0,0,-1,0);
        output[0].w = slice_distance_min - viewport.slice_info.slice_start.z;
        output[1].w = slice_distance_max + viewport.slice_info.slice_start.z;
        break;
    }

    return output;
}


glm::mat4
Nova::ViewportManager::GetViewMatrix() const {
    // Retrieve the view matrix from the active viewport
    if( _activeViewport == GLOBAL_VIEWPORT_ID )
        return glm::mat4();

    const Viewport& viewport = _viewports.at( _activeViewport );
    return viewport.camera->Get_ViewMatrix();
}

glm::mat4
Nova::ViewportManager::GetModelMatrix() const{
    // Retrieve the model matrix from the active viewport
    if( _activeViewport == GLOBAL_VIEWPORT_ID )
        return glm::mat4();


    const Viewport& viewport = _viewports.at( _activeViewport );
    return viewport.camera->Get_ModelMatrix();

}

glm::mat4
Nova::ViewportManager::GetProjectionMatrix() const{
    // Retrieve the projection matrix from the active viewport
    if( _activeViewport == GLOBAL_VIEWPORT_ID )
        return glm::mat4();

    const Viewport& viewport = _viewports.at( _activeViewport );
    return viewport.camera->Get_ProjectionMatrix();

}

glm::vec4
Nova::ViewportManager::GetViewport() const{
    // Retrieve the viewport parameters from the active viewport
    if( _activeViewport == GLOBAL_VIEWPORT_ID )
        return glm::vec4(_global_viewport.x, _global_viewport.y, _global_viewport.width, _global_viewport.height);;

    const Viewport& viewport = _viewports.at( _activeViewport );
    return glm::vec4( viewport.x, viewport.y, viewport.width, viewport.height );
}


glm::mat4
Nova::ViewportManager::GetInteractionViewMatrix() const {
    if( _focused ){
        const Viewport& viewport = _viewports.at( _focusedViewport );
        return viewport.camera->Get_ViewMatrix();
    }
    else{
        const Viewport& viewport = _viewports.at( _recentlyInteractedViewport );
        return viewport.camera->Get_ViewMatrix();
    }       
}

glm::mat4
Nova::ViewportManager::GetInteractionModelMatrix() const {
    if( _focused ){
        const Viewport& viewport = _viewports.at( _focusedViewport );
        return viewport.camera->Get_ModelMatrix();
    }
    else{
        const Viewport& viewport = _viewports.at( _recentlyInteractedViewport );
        return viewport.camera->Get_ModelMatrix();
    }   
}

glm::mat4
Nova::ViewportManager::GetInteractionProjectionMatrix() const {
    if( _focused ){
        const Viewport& viewport = _viewports.at( _focusedViewport );
        return viewport.camera->Get_ProjectionMatrix();
    }
    else{
        const Viewport& viewport = _viewports.at( _recentlyInteractedViewport );
        return viewport.camera->Get_ProjectionMatrix();
    }  
}

glm::vec4
Nova::ViewportManager::GetInteractionViewport() const {
    if( _focused ){
        const Viewport& viewport = _viewports.at( _focusedViewport );
        return glm::vec4( viewport.x, viewport.y, viewport.width, viewport.height );
    }
    else{
        const Viewport& viewport = _viewports.at( _recentlyInteractedViewport );
        return glm::vec4( viewport.x, viewport.y, viewport.width, viewport.height );
    }  
}

void
Nova::ViewportManager::HandleEvent( Nova::IOEvent& event ){
    // The viewport manager's event dispatching call
    Viewport& interacted_viewport = _viewports.at( _recentlyInteractedViewport );

    if( event.type == IOEvent::COMMAND ){
        if( event.command_data->command == "RESET-CAMERA" ){
            if(interacted_viewport.controller)
                interacted_viewport.controller->Reset();
            // Also reset the clipping planes
            interacted_viewport.clipNear = DEFAULT_CLIP_NEAR;
            interacted_viewport.clipFar  = DEFAULT_CLIP_FAR;
        }
        if( event.command_data->command == "WINDOW-RESIZE" ){
            if( event.data_flags | IOEvent::DRAW ){
                this->SetWindowGeometry( event.draw_data->width, event.draw_data->height );
            }
        }
        if( event.command_data->command == "RENDER-FRAME" ){
            for( auto&& v : _viewports )
                if( v.controller )
                    v.controller->Update( event );
        }
        if( event.command_data->command == "SWITCH-CAMERA" ){
            if( event.command_data->args.size() < 1 ){
                std::cout << "Usage: SWITCH-CAMERA <CameraType>" << std::endl;                
            }
            else{                
                // We don't need to disable focus, as the new controller will tell us if we regain it.
                // Remember, by default focus is lost quickly unless it is refreshed every input event.
                // However, if we are focused, the camera controller of the focused viewport is replaced.
                if( _focused ){
                    try{                        
                        Set_Controller( event.command_data->args.at(0), _focusedViewport );
                    }
                    catch( std::exception& e ){
                        std::cout << e.what() << std::endl;
                    }
                }
                // Otherwise, we try to replace the last interacted viewport...
                else{
                    try{                        
                        Set_Controller( event.command_data->args.at(0), _recentlyInteractedViewport );
                    }
                    catch( std::exception& e ){
                        std::cout << e.what() << std::endl;
                    }                    
                }                
            }
        }
        if( event.command_data->command == "VIEWPORT-MODE" ){
            if( event.command_data->args.size() < 1 ){
                std::cout << "Usage: VIEWPORT-MODE <Mode>" << std::endl;                
            }
            if( event.command_data->args[0] == "SINGLE" && _viewports.size() != 1 )
                ConfigureViewports( VM_SINGLE_VIEWPORT );
            if( event.command_data->args[0] == "DUAL" && _viewports.size() != 2 )
                ConfigureViewports( VM_DUAL_VIEWPORT );
            //if( event.command_data->args[0] == "QUAD" && _viewports.size() != 4 )
            //    ConfigureViewports( VM_QUAD_VIEWPORT );
        }

        if( event.command_data->command == "CYCLE-AXIS-MODE" ){
            axis_mode = (axis_mode + 1) % AXIS_MAX_VALUE;
        }

        if( event.command_data->command == "TOGGLE-DEBUG" ){
            if( event.command_data->args.size() >= 1 )
                if( event.command_data->args.at(0) == "FRUSTRUM" )
                    _debug_frustrum = !_debug_frustrum;
        }

        if( event.command_data->command == "TOGGLE-RENDER-STYLE" ){
            int target_viewport = _recentlyInteractedViewport;
            if( _focused )
                target_viewport = _focusedViewport;

            if( event.command_data->args.size() >=1 ){
                if(event.command_data->args.at(0) == "SLICE")
                    Toggle_SliceMode( target_viewport );
                if(event.command_data->args.at(0) == "WIREFRAME"){                    
                    Viewport& viewport = _viewports.at(target_viewport);
                    if( (viewport.render_style & RS_WIREFRAME) && 
                        (viewport.render_style & RS_SOLID) ){
                        viewport.render_style &= ~RS_WIREFRAME;                        
                    }
                    else if( viewport.render_style & RS_WIREFRAME ){
                        viewport.render_style |= RS_SOLID;                        
                    }
                    else if( viewport.render_style & RS_SOLID ){
                        viewport.render_style &= ~RS_SOLID;
                        viewport.render_style |= RS_WIREFRAME;
                    }
                    else
                        viewport.render_style |= RS_SOLID;                    
                }                    
                if(event.command_data->args.at(0) == "LIGHTING"){
                    Viewport& viewport = _viewports.at(target_viewport);
                    viewport.render_style ^= RS_LIGHTING;
                }         
            }

        }

        if( event.command_data->command == "INCREMENT-SLICE" ){
            if( _focused )
                Change_Slice( _focusedViewport, 1 );
            else
                Change_Slice( _recentlyInteractedViewport, 1 );
        }

        if( event.command_data->command == "DECREMENT-SLICE" ){
            if( _focused )
                Change_Slice( _focusedViewport, -1 );
            else
                Change_Slice( _recentlyInteractedViewport, -1 );
        }

        if( event.command_data->command == "SWITCH-SLICE-AXIS" ){
            if( _focused )
                Change_SliceAxis( _focusedViewport );
            else
                Change_SliceAxis( _recentlyInteractedViewport );
        }

    }

    if( event.type == IOEvent::MOUSEMOVE ){
        // Here we will try to find the currently interacting viewport if we are not currently focus locked.
        if( !_focused ){
            // Determine which viewport we are currently interacting with...
            for( int v = 0; v < this->_viewports.size(); v++ ){
                glm::vec2 m = glm::vec2( event.mousemotion_data->x, event.mousemotion_data->y );
                if( m.x >= _viewports[v].x && m.x <= _viewports[v].x+_viewports[v].width &&
                    m.y >= _viewports[v].y && m.y <= _viewports[v].y+_viewports[v].height ){                    
                    _recentlyInteractedViewport = v;
                    break;
                }
            }

            // Send event to the interacting viewport
            if( _viewports.at(_recentlyInteractedViewport).controller)
                _viewports.at(_recentlyInteractedViewport).controller->MouseMove(event);
        }
        else{
            // Otherwise, we dispatch the event to the focused window
            if(_viewports.at( _focusedViewport ).controller)
                _viewports.at( _focusedViewport ).controller->MouseMove( event );
        }
    }

    if( event.type == IOEvent::MOUSEBUTTON ){
        if( !_focused ){
            if( _viewports.at(_recentlyInteractedViewport).controller ){
                if( event.mousebutton_data->action == IOEvent::M_DOWN )
                    _viewports.at(_recentlyInteractedViewport).controller->MouseDown(event);
                if( event.mousebutton_data->action == IOEvent::M_UP )
                    _viewports.at(_recentlyInteractedViewport).controller->MouseUp(event);
            }                                
        }
        else{
            if( _viewports.at(_focusedViewport).controller ){
                if( event.mousebutton_data->action == IOEvent::M_DOWN )
                    _viewports.at(_focusedViewport).controller->MouseDown(event);
                if( event.mousebutton_data->action == IOEvent::M_UP )
                    _viewports.at(_focusedViewport).controller->MouseUp(event);
            }                                
        }
    }

    if( event.type == IOEvent::KEYBOARD ){
        if( !_focused ){
            if( _viewports.at(_recentlyInteractedViewport).controller ){
                if( event.key_data->action == IOEvent::K_DOWN )
                    _viewports.at(_recentlyInteractedViewport).controller->KeyDown(event);
                if( event.key_data->action == IOEvent::K_UP )
                    _viewports.at(_recentlyInteractedViewport).controller->KeyUp(event);
                if( event.key_data->action == IOEvent::K_HOLD )
                    _viewports.at(_recentlyInteractedViewport).controller->KeyHold(event);
            }                                
        }
        else{
            if( _viewports.at(_focusedViewport).controller ){
                if( event.key_data->action == IOEvent::K_DOWN )
                    _viewports.at(_focusedViewport).controller->KeyDown(event);
                if( event.key_data->action == IOEvent::K_UP )
                    _viewports.at(_focusedViewport).controller->KeyUp(event);
                if( event.key_data->action == IOEvent::K_HOLD )
                    _viewports.at(_focusedViewport).controller->KeyHold(event);
            }                                
        }
    }
    
    if( event.type == IOEvent::SCROLL ){
        int pickedviewport = _recentlyInteractedViewport;
        if( _focused)
            pickedviewport = _focusedViewport;

        if( event.scroll_data->mods == ( IOEvent::mCONTROL | IOEvent::mSHIFT ) ){
            float log_of_clip = std::max( std::log( _viewports.at(pickedviewport).clipNear ), 0.1f );
            _viewports.at(pickedviewport).clipNear += (event.scroll_data->y * log_of_clip);
            _viewports.at(pickedviewport).clipNear = std::min( std::max( DEFAULT_CLIP_NEAR, _viewports.at(pickedviewport).clipNear), DEFAULT_CLIP_FAR );
        }
        else if( event.scroll_data->mods == ( IOEvent::mCONTROL | IOEvent::mALT ) ){
            float log_of_clip = std::max( std::log( _viewports.at(pickedviewport).clipFar ), 0.1f );
            _viewports.at(pickedviewport).clipFar += event.scroll_data->y * log_of_clip;
            _viewports.at(pickedviewport).clipFar = std::min( std::max( DEFAULT_CLIP_NEAR, _viewports.at(pickedviewport).clipFar), DEFAULT_CLIP_FAR );
        }
        else if( _viewports.at(pickedviewport).controller )
            _viewports.at(pickedviewport).controller->Scroll(event);
    }

    
    // Check for viewport focus. Assume nothing is focused unless we are told otherwise
    bool _prevFocus = _focused;
    _focused = false;
    for( int v = 0; v < this->_viewports.size(); v++ ){
        if( _viewports.at( v ).controller)
            if( _viewports.at( v ).controller->HasFocus()){
                _focused = true;
                _focusedViewport = v;
            }                
    }
}


