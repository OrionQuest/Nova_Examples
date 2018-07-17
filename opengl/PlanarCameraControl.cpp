#include "PlanarCameraControl.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp> 
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <iostream>


Nova::PlanarCameraControl::PlanarCameraControl( Camera& camera ) : BASE( camera ) 
{
    enabled = true;
    target = GetCamera().Get_Look_At();
    position = GetCamera().Get_Position();
    minZoom = 0;
    maxZoom = 1e8;
    zoom = 1.0;
    zoomSpeed = 1.0f;
    zoomChanged = false;

    position0 = GetCamera().Get_Position();    
    target0 = GetCamera().Get_Look_At();
    zoom0 = 1.0f;

    state = NONE;
    EPS = 0.000001;
}


bool
Nova::PlanarCameraControl::HasFocus(){
    if( state != NONE )
        return true;
    else
        return false;

}

void Nova::PlanarCameraControl::MouseDown(const Nova::IOEvent& event) {

    if ( enabled == false ) return;
    switch( event.mousebutton_data->button ){
    case Nova::IOEvent::M_LEFT:
        if( event.mousebutton_data->mods & IOEvent::mSHIFT ) return;
        break;
    case Nova::IOEvent::M_RIGHT:
        panStart = glm::vec2( event.mousebutton_data->x, event.mousebutton_data->y );
        state = PAN;
        break;
    case Nova::IOEvent::M_MIDDLE:
        dollyStart = glm::vec2( event.mousebutton_data->x, event.mousebutton_data->y );
        state = DOLLY;
        break;
    }
};

void Nova::PlanarCameraControl::MouseUp(const Nova::IOEvent& event) {
    if ( enabled == false ) return;
    state = NONE;
};

void Nova::PlanarCameraControl::MouseMove(const Nova::IOEvent& event) {
    if ( enabled == false ) return;
    if ( state == ROTATE ) {
        // Nothing here!
    } else if ( state == DOLLY ) {
        dollyEnd = glm::vec2( event.mousemotion_data->x, event.mousemotion_data->y );
		dollyDelta = dollyEnd - dollyStart;
        if ( dollyDelta.y < 0 ) {
            dollyIn( getZoomScale() );            
		} else if ( dollyDelta.y > 0 ) {
			dollyOut( getZoomScale() );
		}
        
		dollyStart = dollyEnd;
    } else if ( state == PAN ) {
        panEnd = glm::vec2( event.mousemotion_data->x, event.mousemotion_data->y );
		panDelta = panEnd - panStart;
		pan( panDelta.x, panDelta.y );
		panStart = panEnd;
    }
    Update(event);
};

void Nova::PlanarCameraControl::KeyDown(const Nova::IOEvent& event){
};

void Nova::PlanarCameraControl::KeyUp(const Nova::IOEvent& event) {    
};

void Nova::PlanarCameraControl::KeyHold(const Nova::IOEvent& event) {


};


void Nova::PlanarCameraControl::Redraw(const Nova::IOEvent& event) {
    screen.left = event.draw_data->x;
    screen.top = event.draw_data->y;
    screen.width = event.draw_data->width;
    screen.height = event.draw_data->height;
};

void Nova::PlanarCameraControl::Scroll(const Nova::IOEvent& event) {
    if ( enabled == false || ( state != NONE && state != ROTATE ) ) return;       
    if ( event.scroll_data->y < 0 ) {
        dollyOut( getZoomScale() );
    } else if ( event.scroll_data->y > 0 ) {        
        dollyIn( getZoomScale() );        
    }
    Update(event);
}

// State Methods
void Nova::PlanarCameraControl::Reset()
{
    target = target0;
    zoom = zoom0;

    GetCamera().Set_Position(position0);
    GetCamera().Set_Look_At( target0 );
    GetCamera().Set_Scale( zoom );

    GetCamera().Set_Mode(ORTHO);
    state = NONE;
};

// glm::quat fromUnitVectors( const glm::vec3& u, const glm::vec3& v ){
//     // Math reference:
//     // http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
//     float m = glm::sqrt(2.f + 2.f * glm::dot(u, v));
//     glm::vec3 w = (1.f / m) * glm::cross(u, v);
//     return glm::quat(0.5f * m, w.x, w.y, w.z);
// }

// glm::vec3 toSpherical( glm::vec3 e ){
//   float r = glm::length(e);
//   if( r == 0 ){
//     return glm::vec3(r, 0, 0);
//   }
//   else{
//     return glm::vec3( r, glm::atan(e.x,e.z), glm::acos( glm::clamp( e.y/r, -1.0f, 1.0f) ) );
//   }
// }

// glm::vec3 toEuclidean( glm::vec3 s){

//   float sinPhiRadius = glm::sin( s.z ) * s.x;
//   return glm::vec3( sinPhiRadius * glm::sin( s.y ),
// 		    glm::cos( s.z ) * s.x,
// 		    sinPhiRadius * glm::cos( s.y ) );
// }

void Nova::PlanarCameraControl::Update(const Nova::IOEvent& event) {

    position = GetCamera().Get_Position();

    position += panOffset;
    target += panOffset;
    
    GetCamera().Set_Position( position );
    GetCamera().Set_Look_At( target );

    panOffset= glm::vec3( 0, 0, 0 );

    if ( zoomChanged ){
        GetCamera().Set_Scale( zoom );
        zoomChanged = false;
    }

};

/*
float Nova::PlanarCameraControl::getAutoRotationAngle() {
    return 2 * glm::pi<float>() / 60 / 60 * autoRotateSpeed;
}
*/

float Nova::PlanarCameraControl::getZoomScale() {
    return glm::pow( 0.95f, zoomSpeed );
}

/*
void Nova::PlanarCameraControl::rotateLeft( float angle ) {
    sphericalDelta.y -= angle;
}

void Nova::PlanarCameraControl::rotateUp( float angle ) {
    sphericalDelta.z -= angle;
}
*/

void Nova::PlanarCameraControl::panLeft( float distance, glm::mat4 objectMatrix ){
    glm::vec3 v;    
    glm::vec3 translation_x = glm::vec3( -distance, 0, 0);
    v = glm::inverse( glm::mat3(objectMatrix) ) * translation_x;
    panOffset += v;    
}

void Nova::PlanarCameraControl::panUp( float distance, glm::mat4 objectMatrix ){
    glm::vec3 v;    
    glm::vec3 translation_y = glm::vec3( 0, distance, 0 );
    v = glm::inverse( glm::mat3(objectMatrix) ) * translation_y;
    panOffset += v;    
}

void Nova::PlanarCameraControl::pan( float deltaX, float deltaY ){
    glm::vec3 offset;
    
    std::array<float,6> ortho_coords = GetCamera().Get_Ortho_Box();
    // orthographic
    panLeft( deltaX * (ortho_coords[1]-ortho_coords[0])/screen.width, GetCamera().Get_ModelMatrix());
    panUp( deltaY * ( ortho_coords[3]-ortho_coords[2] )/screen.width, GetCamera().Get_ModelMatrix());    
}


void Nova::PlanarCameraControl::dollyIn( float dollyScale ) {
    zoom = glm::max( minZoom, glm::min( maxZoom, zoom * dollyScale ) );
    zoomChanged = true;                
}

void Nova::PlanarCameraControl::dollyOut( float dollyScale ) {
    zoom = glm::max( minZoom, glm::min( maxZoom, zoom / dollyScale ) );
    zoomChanged = true;                
}

