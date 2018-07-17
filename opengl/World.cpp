//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
#include "World.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "ApplicationFactory.h"
#include "OpenGLState.h"

#include "IOEvent.h"
#include "ViewportManager.h"

using namespace Nova;
//#####################################################################
// Constructor
//#####################################################################
World::
World(ApplicationFactory& app)
    :_app(app),window(nullptr)//,control(nullptr)
{

     viewport = std::unique_ptr<ViewportManager>( new ViewportManager(app) );
     _app.GetIOService().On( IOService::MOUSE_DOWN, [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( IOService::MOUSE_UP, [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( IOService::MOUSE_MOVE, [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( IOService::SCROLL, [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( IOService::KEY_DOWN, [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( IOService::KEY_UP, [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( IOService::KEY_HOLD, [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( "WINDOW-RESIZE", [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( "RENDER-FRAME", [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( "RESET-CAMERA", [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( "SWITCH-CAMERA", [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( "VIEWPORT-MODE", [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( "CYCLE-AXIS-MODE", [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( "INCREMENT-SLICE", [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( "DECREMENT-SLICE", [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( "SWITCH-SLICE-AXIS", [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( "TOGGLE-RENDER-STYLE", [&](IOEvent& event){viewport->HandleEvent(event);} );
     _app.GetIOService().On( "TOGGLE-DEBUG", [&](IOEvent& event){viewport->HandleEvent(event);} );

}
//#####################################################################
// Destructor
//#####################################################################
World::
~World()
{
glfwTerminate();
//if( control ){
//delete control;
//control = nullptr;
//}
}
//#####################################################################
// Camera Matrix Operators
//#####################################################################
glm::mat4 
World::Get_ViewMatrix(MATRIX_MODE mode) const {
    if( mode == DRAW )
        return viewport->GetViewMatrix();
    else
        return viewport->GetInteractionViewMatrix();
}

glm::mat4
World::Get_ModelMatrix(MATRIX_MODE mode) const {
    if( mode == DRAW )
        return viewport->GetModelMatrix();
    else
        return viewport->GetInteractionModelMatrix();
}

glm::mat4
World::Get_ProjectionMatrix(MATRIX_MODE mode) const {
    if( mode == DRAW )
        return viewport->GetProjectionMatrix();
    else
    return viewport->GetInteractionProjectionMatrix();
}

glm::vec4 World::Get_Viewport(MATRIX_MODE mode) const {
    if( mode == DRAW )
        return viewport->GetViewport();
    else
        return viewport->GetInteractionViewport();
}

std::array< glm::vec4, 2 >
World::Get_Slice_Planes() const {
    return viewport->GetSlicePlanes();
}

std::array< glm::vec4, 2 >
World::Get_Slice_Planes(unsigned int v) const {
    return viewport->GetSlicePlanes(v);
}

//#####################################################################
// State Queries
//#####################################################################

bool
World::SliceMode() const {
    return viewport->RenderStyle( ViewportManager::RS_SLICE );
}
bool
World::SliceMode(unsigned int v) const {
    return viewport->RenderStyle( ViewportManager::RS_SLICE,  v);
}
bool
World::WireframeMode() const {
    return viewport->RenderStyle( ViewportManager::RS_WIREFRAME );
}
bool
World::WireframeMode(unsigned int v) const {
    return viewport->RenderStyle( ViewportManager::RS_WIREFRAME,  v );
}
bool
World::SolidMode() const {
    return viewport->RenderStyle( ViewportManager::RS_SOLID );
}
bool
World::SolidMode(unsigned int v) const {
    return viewport->RenderStyle( ViewportManager::RS_SOLID,  v );
}
bool
World::LightingMode() const {
    return viewport->RenderStyle( ViewportManager::RS_LIGHTING );
}
bool
World::LightingMode(unsigned int v) const {
    return viewport->RenderStyle( ViewportManager::RS_LIGHTING,  v );
}
//#####################################################################
// Lighting Information
//#####################################################################

std::vector< glm::vec3 >
World::GetSceneLightPositions() const {
    std::vector<glm::vec3> lights;

    // We want some more complicated light configurations later...
    lights.push_back( GetCameraPosition() );
    return lights;
}

glm::vec3
World::GetCameraPosition() const {
    return viewport->GetCameraPosition();
}

//#####################################################################
// Window Information
//#####################################################################
glm::ivec2
World::WindowSize() const
{
return window_size;
}

glm::ivec2
World::WindowPosition() const
{
return window_position;
}

glm::ivec2
World::ScreenSize() const
{
return window_physical_size;
}

float
World::Get_DPI() const
{
return float(dpi);
}

//#####################################################################
// Initialize
//#####################################################################
ViewportManager&
    World::GetViewportManager(){
return *( viewport.get() );
}


//#####################################################################
// Initialize
//#####################################################################
void World::
Initialize(int size_x,int size_y)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    window=glfwCreateWindow(size_x,size_y, (std::string("OpenGL Visualization:   ") + _app.GetConfig().scenepath).c_str(), nullptr,nullptr);
    if(window==nullptr)
    {
        std::cout<<"Failed to create GLFW window!"<<std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    // disable vsync
    glfwSwapInterval(0);

    glewExperimental=GL_TRUE;
    if(glewInit()!=GLEW_OK)
    {
        std::cout<<"Failed to initialize GLEW!"<<std::endl;
        return;
    }

    glfwSetWindowCloseCallback(window,Close_Callback);
    glfwSetFramebufferSizeCallback(window,Reshape_Callback);
    glfwSetKeyCallback(window,Keyboard_Callback);
    glfwSetMouseButtonCallback(window,Mouse_Button_Callback);
    glfwSetCursorPosCallback(window,Mouse_Position_Callback);
    glfwSetScrollCallback(window,Scroll_Callback);

    glfwGetFramebufferSize(window,&size_x,&size_y);
    if(size_y>0)
    {
        window_size=glm::ivec2(size_x,size_y);
        window_aspect=float(size_x)/float(size_y);

        GLFWmonitor* primary=glfwGetPrimaryMonitor();
        glfwGetMonitorPhysicalSize(primary,&window_physical_size.x,&window_physical_size.y);
        glfwGetMonitorPos(primary,&window_position.x,&window_position.y);
        const GLFWvidmode* mode=glfwGetVideoMode(primary);
        dpi=mode->width/(window_physical_size.x/25.4);
    }

    glfwSetWindowUserPointer(window,this);
    glfwSetInputMode(window,GLFW_CURSOR_DISABLED,GL_FALSE);
}
//#####################################################################
// Initialize_Camera_Controls
//#####################################################################
void World::
Initialize_Camera_Controls()
{
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  // Fire a redraw event to initialize the control to the window size,
  // just in case it needs it. 
  //IOEvent event(IOEvent::DRAW);
  //event.draw_data->width = width;
  //event.draw_data->height = height;
  //event.currentTime = glfwGetTime();
  //control->Redraw( event );

  // Call reset to clear any state
  //control->Reset();
  viewport->ConfigureViewports(ViewportManager::VM_SINGLE_VIEWPORT );
  viewport->InitializeChrome();
  viewport->SetWindowGeometry( width, height );
  viewport->ResetAll();
}
//#####################################################################
// Handle_Input
//#####################################################################
void World::
Handle_Input(int key)
{
}
//#####################################################################
// Main_Loop
//#####################################################################
void World::
Main_Loop()
{
    Initialize_Camera_Controls();
    _app.GetIOService().ClearTime();
    glfwSetTime(0.0);
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        GLStateEnable enableBlend(GL_BLEND);
        GLStateEnable enableDepth(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //GLStateEnable enableCull(GL_CULL_FACE);

        glClearColor(18.0f/255.0f,26.0f/255.0f,32.0f/255.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);


        viewport->SetGlobalViewport();
        viewport->Update();
        IOEvent event(IOEvent::TIME);
        event.currentTime = glfwGetTime();
        _app.GetIOService().Trigger( event );      
        _app.GetIOService().SetTime( event.currentTime ); // Set a new time for
                                                          // the timers
        
        // For every viewport, update its camera and draw the scene.
        for( int v = 0; v < viewport->NumViewports(); v++ ){
            viewport->SetViewport(v);           
            viewport->Update();

            if( viewport->RenderStyle(ViewportManager::RS_SLICE) ){
                GLStateEnable enableClip0(GL_CLIP_DISTANCE0);
                GLStateEnable enableClip1(GL_CLIP_DISTANCE1);
                _app.GetScene().Draw();
            }
            else{
                GLStateDisable enableClip0(GL_CLIP_DISTANCE0);
                GLStateDisable enableClip1(GL_CLIP_DISTANCE1);
                _app.GetScene().Draw();
            }

            
            viewport->DrawFrustrums();
            // This method messes with the viewport, so call it last...
            viewport->DrawAxis();
        }
        
        viewport->SetGlobalViewport();
        viewport->Update();

        // Draw any text that has been buffered so far this frame
        _app.GetTextRenderingService().Flush();
        
        glClear(GL_DEPTH_BUFFER_BIT); // Clear the depth buffer as the frame is not used to clip things
        viewport->DrawFrame();
                
        glfwSwapBuffers(window);
    }
}
//#####################################################################

void World::Close_Callback(GLFWwindow* window)
{glfwSetWindowShouldClose(window,GL_TRUE);}

void World::Scroll_Callback( GLFWwindow* window,double xoffset,double yoffset)
{
    World *world=static_cast<World*>(glfwGetWindowUserPointer(window));
    
    IOEvent event(IOEvent::SCROLL);
    event.scroll_data->x = xoffset;
    event.scroll_data->y = yoffset;
    event.currentTime = glfwGetTime();


    int shift_state = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 1 : 0) || 
                      (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS ? 1 : 0);
    int ctrl_state  = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ? 1 : 0) || 
                      (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS ? 1 : 0);
    int alt_state   = (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ? 1 : 0) || 
                      (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS ? 1 : 0);
    int super_state = (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS ? 1 : 0) || 
                      (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS ? 1 : 0);
    event.scroll_data->mods = (shift_state * IOEvent::mSHIFT) | 
                                   (ctrl_state * IOEvent::mCONTROL) | 
                                   (alt_state * IOEvent::mALT) | 
                                   (super_state * IOEvent::mSUPER);

    world->_app.GetIOService().Trigger( event );
    
}


void World::Reshape_Callback(GLFWwindow* window,int w,int h)
{
    World *world=static_cast<World*>(glfwGetWindowUserPointer(window));
    if(h>0)
        {
            world->window_size=glm::ivec2(w,h);
            world->window_aspect=float(w)/float(h);
        }
    
    GLFWmonitor *primary=glfwGetPrimaryMonitor();
    glfwGetMonitorPhysicalSize(primary,&world->window_physical_size.x,&world->window_physical_size.y);
    glfwGetMonitorPos(primary,&world->window_position.x,&world->window_position.y);    
    const GLFWvidmode *mode=glfwGetVideoMode(primary);
    world->dpi=mode->width/(world->window_physical_size.x/25.4);
    
    IOEvent event(IOEvent::DRAW);
    event.draw_data->width = w;
    event.draw_data->height = h;
    event.currentTime = glfwGetTime();
    world->_app.GetIOService().Trigger( event );
}

void World::Keyboard_Callback(GLFWwindow* window,int key,int scancode,int action,int mode)
{
    World *world=static_cast<World*>(glfwGetWindowUserPointer(window));
    if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS)
        glfwSetWindowShouldClose(window,GL_TRUE);           
    
    IOEvent event(IOEvent::KEYBOARD);
    event.key_data->key = IOEvent::KEY_CODE(key);
    event.key_data->scancode = scancode;
    event.key_data->mods = 0;
    if( mode & GLFW_MOD_SHIFT )
        event.key_data->mods |= IOEvent::mSHIFT;
    if( mode & GLFW_MOD_CONTROL )
        event.key_data->mods |= IOEvent::mCONTROL;
    if( mode & GLFW_MOD_ALT )
        event.key_data->mods |= IOEvent::mALT;
    if( mode & GLFW_MOD_SUPER )
        event.key_data->mods |= IOEvent::mSUPER;

    event.currentTime = glfwGetTime();
    switch( action ){
    case GLFW_PRESS:
        event.key_data->action = IOEvent::K_DOWN;
        break;
    case GLFW_RELEASE:
        event.key_data->action = IOEvent::K_UP;
        break;
    case GLFW_REPEAT:
        event.key_data->action = IOEvent::K_HOLD;
        break;
    }

    world->_app.GetIOService().Trigger( event );
}

void World::Mouse_Button_Callback(GLFWwindow* window,int button,int state,int mode)
{
    World *world=static_cast<World*>(glfwGetWindowUserPointer(window));
    double x,y;
    glfwGetCursorPos(window,&x,&y);
    
    IOEvent event(IOEvent::MOUSEBUTTON);
    event.currentTime = glfwGetTime();
    switch( button ){
    case GLFW_MOUSE_BUTTON_LEFT:
        event.mousebutton_data->button = IOEvent::M_LEFT;
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        event.mousebutton_data->button = IOEvent::M_RIGHT;
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        event.mousebutton_data->button = IOEvent::M_MIDDLE;
        break;
    default:
        event.mousebutton_data->button = IOEvent::M_OTHER;
        break;
    }
    event.mousebutton_data->button_raw = button;
    event.mousebutton_data->x = x;
    event.mousebutton_data->y = y;
    event.mousebutton_data->mods = 0;
    if( mode & GLFW_MOD_SHIFT )
        event.mousebutton_data->mods |= IOEvent::mSHIFT;
    if( mode & GLFW_MOD_CONTROL )
        event.mousebutton_data->mods |= IOEvent::mCONTROL;
    if( mode & GLFW_MOD_ALT )
        event.mousebutton_data->mods |= IOEvent::mALT;
    if( mode & GLFW_MOD_SUPER )
        event.mousebutton_data->mods |= IOEvent::mSUPER;

    switch( state ){
    case GLFW_PRESS:
        event.mousebutton_data->action = IOEvent::M_DOWN;
        break;
    case GLFW_RELEASE:
        event.mousebutton_data->action = IOEvent::M_UP;
        break;
    }
    world->_app.GetIOService().Trigger( event );
}

void World::Mouse_Position_Callback(GLFWwindow* window,double x,double y)
{
    World *world=static_cast<World*>(glfwGetWindowUserPointer(window));
    IOEvent event(IOEvent::MOUSEMOVE);

    int shift_state = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 1 : 0) || 
                      (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS ? 1 : 0);
    int ctrl_state  = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ? 1 : 0) || 
                      (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS ? 1 : 0);
    int alt_state   = (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ? 1 : 0) || 
                      (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS ? 1 : 0);
    int super_state = (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS ? 1 : 0) || 
                      (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS ? 1 : 0);

    event.currentTime = glfwGetTime();
    event.mousemotion_data->x = x;
    event.mousemotion_data->y = y;
    event.mousemotion_data->mods = (shift_state * IOEvent::mSHIFT) | 
                                   (ctrl_state * IOEvent::mCONTROL) | 
                                   (alt_state * IOEvent::mALT) | 
                                   (super_state * IOEvent::mSUPER);
    world->_app.GetIOService().Trigger( event );
}
