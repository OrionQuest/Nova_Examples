#include "OrbitCameraControl.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp> 
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <iostream>


Nova::OrbitCameraControl::OrbitCameraControl( Camera& camera ) : BASE( camera ) 
{
    enabled = true;
    target = GetCamera().Get_Look_At();
    minDistance = 0;
    maxDistance = 1e8;
    minZoom = 0;
    maxZoom = 1e8;
    minPolarAngle = 0; // radians
    maxPolarAngle = glm::pi<float>(); // radians
    minAzimuthAngle = -1e8; // radians
    maxAzimuthAngle = 1e8; // radians
    enableDamping = false;
    dampingFactor = 0.25;
    enableZoom = true;
    zoomSpeed = 1.0;
    enableRotate = true;
    rotateSpeed = 1.0;
    enablePan = true;
    keyPanSpeed = 7.0;	// pixels moved per arrow key push
    autoRotate = false;
    autoRotateSpeed = 2.0; // 30 seconds per round when fps is 60
    enableKeys = true;
    keys = {IOEvent::KEY_W, IOEvent::KEY_S, IOEvent::KEY_A, IOEvent::KEY_D };
    zoom = 1.0;

    position0 = GetCamera().Get_Position();    
    target0 = GetCamera().Get_Look_At();
    zoom0 = 1.0f;

    state = NONE;
    EPS = 0.000001;
    scale = 1;
    zoomChanged = false;   
}


bool
Nova::OrbitCameraControl::HasFocus(){
    if( state != NONE )
        return true;
    else
        return false;

}

void Nova::OrbitCameraControl::MouseDown(const Nova::IOEvent& event) {

    if ( enabled == false ) return;
    switch( event.mousebutton_data->button ){
    case Nova::IOEvent::M_LEFT:
        if( event.mousebutton_data->mods & IOEvent::mSHIFT ) return;
        if ( enableRotate == false ) return;
        rotateStart = glm::vec2( event.mousebutton_data->x, event.mousebutton_data->y );
        state = ROTATE;
        break;
    case Nova::IOEvent::M_RIGHT:
        if ( enablePan == false ) return;
        panStart = glm::vec2( event.mousebutton_data->x, event.mousebutton_data->y );
        state = PAN;
        break;
    case Nova::IOEvent::M_MIDDLE:
        if ( enableZoom == false ) return;
        dollyStart = glm::vec2( event.mousebutton_data->x, event.mousebutton_data->y );
        state = DOLLY;
        break;
    default: break;
    }
};

void Nova::OrbitCameraControl::MouseUp(const Nova::IOEvent& event) {
    if ( enabled == false ) return;
    state = NONE;
};

void Nova::OrbitCameraControl::MouseMove(const Nova::IOEvent& event) {
    if ( enabled == false ) return;
    if ( state == ROTATE ) {
        if ( enableRotate == false ) return;
        
        rotateEnd = glm::vec2( event.mousemotion_data->x, event.mousemotion_data->y );
		rotateDelta = rotateEnd - rotateStart;
		// rotating across whole screen goes 360 degrees around
		rotateLeft( 2 * glm::pi<float>() * rotateDelta.x / screen.width * rotateSpeed );

		// rotating up and down along whole screen attempts to go 360, but limited to 180
		rotateUp( 2 * glm::pi<float>() * rotateDelta.y / screen.width * rotateSpeed );

		rotateStart = rotateEnd;
    } else if ( state == DOLLY ) {
        if ( enableZoom == false ) return;
        dollyEnd = glm::vec2( event.mousemotion_data->x, event.mousemotion_data->y );
		dollyDelta = dollyEnd - dollyStart;
        
		if ( dollyDelta.y > 0 ) {
            dollyIn( getZoomScale() );            
		} else if ( dollyDelta.y < 0 ) {
			dollyOut( getZoomScale() );
		}
		dollyStart = dollyEnd;
    } else if ( state == PAN ) {
        if ( enablePan == false ) return;
        panEnd = glm::vec2( event.mousemotion_data->x, event.mousemotion_data->y );
		panDelta = panEnd - panStart;
		pan( panDelta.x, panDelta.y );
		panStart = panEnd;
    }
    Update(event);
};

void Nova::OrbitCameraControl::KeyDown(const Nova::IOEvent& event) {
};

void Nova::OrbitCameraControl::KeyUp(const Nova::IOEvent& event) {    
};

void Nova::OrbitCameraControl::KeyHold(const Nova::IOEvent& event) {
    if ( enabled == false || enableKeys == false || enablePan == false ) return;

    if( event.key_data->key ==  keys[0]) { // Up
        pan( 0, keyPanSpeed );
        Update(event);       
    } else if (  event.key_data->key == keys[1] ){ // Down
        pan( 0, -keyPanSpeed );
        Update(event);
    } else if(   event.key_data->key == keys[2] ){ // Left
        pan( keyPanSpeed, 0 );
        Update(event);
    }else if (   event.key_data->key == keys[3] ){ // Right     
        pan( -keyPanSpeed, 0 );
        Update(event);
    }    
};

void Nova::OrbitCameraControl::Redraw(const Nova::IOEvent& event) {
    screen.left = event.draw_data->x;
    screen.top = event.draw_data->y;
    screen.width = event.draw_data->width;
    screen.height = event.draw_data->height;
};

void Nova::OrbitCameraControl::Scroll(const Nova::IOEvent& event) {
    if ( enabled == false || enableZoom == false || ( state != NONE && state != ROTATE ) ) return;       
    if ( event.scroll_data->y < 0 ) {
        dollyOut( getZoomScale() );
    } else if ( event.scroll_data->y > 0 ) {        
        dollyIn( getZoomScale() );        
    }
    Update(event);
}

// State Methods
void Nova::OrbitCameraControl::Reset()
{
    target = target0;
    zoom = zoom0;

    GetCamera().Set_Position(position0);
    GetCamera().Set_Look_At( target0 );
    GetCamera().Set_Mode(FREE);
    state = NONE;
};

glm::quat fromUnitVectors( const glm::vec3& u, const glm::vec3& v ){
    // Math reference:
    // http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
    float m = glm::sqrt(2.f + 2.f * glm::dot(u, v));
    glm::vec3 w = (1.f / m) * glm::cross(u, v);
    return glm::quat(0.5f * m, w.x, w.y, w.z);
}

glm::vec3 toSpherical( glm::vec3 e ){
  float r = glm::length(e);
  if( r == 0 ){
    return glm::vec3(r, 0, 0);
  }
  else{
    return glm::vec3( r, glm::atan(e.x,e.z), glm::acos( glm::clamp( e.y/r, -1.0f, 1.0f) ) );
  }
}

glm::vec3 toEuclidean( glm::vec3 s){

  float sinPhiRadius = glm::sin( s.z ) * s.x;
  return glm::vec3( sinPhiRadius * glm::sin( s.y ),
		    glm::cos( s.z ) * s.x,
		    sinPhiRadius * glm::cos( s.y ) );
}

void Nova::OrbitCameraControl::Update(const Nova::IOEvent& event) {

    glm::vec3 offset;
    
    // so camera.up is the orbit axis
    glm::quat quat;
    quat = fromUnitVectors( GetCamera().Get_Up(), glm::vec3(0,1,0) );
    glm::quat quatInverse = glm::inverse(quat);
    glm::vec3 lastPosition;
    glm::quat lastQuaternion;

        
    offset = GetCamera().Get_Position() - target;
 
    // rotate offset to "y-axis-is-up" space
    offset = offset * quat;
        
    // angle from z-axis around y-axis
    spherical = toSpherical( offset );
  
    if ( autoRotate && state == NONE ) {
        rotateLeft( getAutoRotationAngle() );        
    }
    // x is radius, y is theta, z is phi
    spherical.y += sphericalDelta.y;
    spherical.z += sphericalDelta.z;

     
    // restrict theta to be between desired limits
    spherical.y =glm::max( minAzimuthAngle, glm::min( maxAzimuthAngle, spherical.y ) );
        
    // restrict phi to be between desired limits
    spherical.z = glm::max( minPolarAngle, glm::min( maxPolarAngle, spherical.z ) );
        
    spherical.z = glm::max( EPS, glm::min( glm::pi<float>() - EPS, spherical.z ) );
    
    spherical.x *= scale;
        
    // restrict radius to be between desired limits
    spherical.x = glm::max( minDistance, glm::min( maxDistance, spherical.x ) );

      // move target to panned location
    target += panOffset;
        
    offset = toEuclidean( spherical );

    // rotate offset back to "camera-up-vector-is-up" space
    offset = offset * quatInverse;
    
    GetCamera().Set_Position( target + offset );
        
    GetCamera().Set_Look_At( target );
        
    if ( enableDamping == true ) {
        sphericalDelta.y *= ( 1 - dampingFactor );
        sphericalDelta.z *= ( 1 - dampingFactor );
    } else {
        sphericalDelta = glm::vec3(0,0,0);
    }
        
    scale = 1;
    panOffset= glm::vec3( 0, 0, 0 );
        
    // update condition is:
    // min(camera displacement, camera rotation in radians)^2 > EPS
    // using small-angle approximation cos(x/2) = 1 - x^2 / 8
        
    if ( zoomChanged ||
         glm::length2( lastPosition - GetCamera().Get_Position() ) > EPS ||
         8 * ( 1 - glm::dot(lastQuaternion, GetCamera().Get_Rotation() )) > EPS ) {      

        GetCamera().Set_Scale( zoom );
        lastPosition = GetCamera().Get_Position();
        lastQuaternion = GetCamera().Get_Rotation();
        zoomChanged = false;                  
    }    
};

float Nova::OrbitCameraControl::getAutoRotationAngle() {
    return 2 * glm::pi<float>() / 60 / 60 * autoRotateSpeed;
}

float Nova::OrbitCameraControl::getZoomScale() {
    return glm::pow( 0.95f, zoomSpeed );
}

void Nova::OrbitCameraControl::rotateLeft( float angle ) {
    sphericalDelta.y -= angle;
}

void Nova::OrbitCameraControl::rotateUp( float angle ) {
    sphericalDelta.z -= angle;
}

 void Nova::OrbitCameraControl::panLeft( float distance, glm::mat4 objectMatrix ){
     glm::vec3 v;    
     //v = glm::vec3(glm::column( objectMatrix, 0 )); // get X column of objectMatrix
     //v *= (-distance);
     glm::vec3 translation_x = glm::vec3( -distance, 0, 0);
     v = glm::inverse( glm::mat3(objectMatrix) ) * translation_x;
     panOffset += v;    
}

void Nova::OrbitCameraControl::panUp( float distance, glm::mat4 objectMatrix ){
    glm::vec3 v;    
    //v = glm::vec3(glm::column( objectMatrix, 1 )); // get Y column of objectMatrix
    //v *= (distance);
    glm::vec3 translation_y = glm::vec3( 0, distance, 0 );
     v = glm::inverse( glm::mat3(objectMatrix) ) * translation_y;
    panOffset += v;    
}

void Nova::OrbitCameraControl::pan( float deltaX, float deltaY ){
    glm::vec3 offset;
    
    if ( GetCamera().Get_Mode() == FREE ) {
        // perspective
        glm::vec3 position = GetCamera().Get_Position();
        offset = position - target;
        float targetDistance = glm::length(offset);
        
        // half of the fov is center to top of screen
        targetDistance *= glm::tan( ( GetCamera().Get_FOV() / 2 ) * glm::pi<float>() / 180.0 );
        
        // we actually don't use screenWidth, since perspective camera is fixed to screen height
        panLeft( 2 * deltaX * targetDistance / screen.height, GetCamera().Get_ViewMatrix());
        panUp( 2 * deltaY * targetDistance / screen.height, GetCamera().Get_ViewMatrix());
        
    } else if ( GetCamera().Get_Mode() == ORTHO  ) {
        std::array<float,6> ortho_coords = GetCamera().Get_Ortho_Box();
        // orthographic
        panLeft( deltaX * (ortho_coords[1]-ortho_coords[0])/screen.width, GetCamera().Get_ModelMatrix());
        panUp( deltaY * ( ortho_coords[3]-ortho_coords[2] )/screen.width, GetCamera().Get_ModelMatrix());
    }
}

void Nova::OrbitCameraControl::dollyIn( float dollyScale ) {
    //if ( GetCamera().Get_Mode() == FREE ){
        scale /= dollyScale;        
        //} else if ( GetCamera().Get_Mode() == ORTHO ) {
        zoom = glm::max( minZoom, glm::min( maxZoom, zoom * dollyScale ) );
        // scope.object.updateProjectionMatrix(); ???
        zoomChanged = true;        
        //}    
}

void Nova::OrbitCameraControl::dollyOut( float dollyScale ) {
    //if ( GetCamera().Get_Mode() == FREE ){
        scale *= dollyScale;        
        //} else if ( GetCamera().Get_Mode() == ORTHO ) {
        zoom = glm::max( minZoom, glm::min( maxZoom, zoom / dollyScale ) );
        // scope.object.updateProjectionMatrix(); ???
        zoomChanged = true;        
        //}    
}
