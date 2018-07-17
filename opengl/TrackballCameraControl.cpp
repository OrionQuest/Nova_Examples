#include "TrackballCameraControl.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

Nova::TrackballCameraControl::TrackballCameraControl( Camera& camera ) : BASE( camera ) 
{
    enabled = true;
    screen.left = 0;
    screen.top = 0;
    screen.width = 0;
    screen.height = 0;
    rotateSpeed = 1.0;
    zoomSpeed = 1.2;
    panSpeed = 0.3;
    noRotate = false;
    noZoom = false;
    noPan = false;
    staticMoving = false;
    dynamicDampingFactor = 0.2;
    minDistance = 0;
    maxDistance = 1e8;
    keys = { IOEvent::KEY_A, IOEvent::KEY_W,  IOEvent::KEY_D };
    _state = NONE;
    _prevState = NONE;
    _useFixedAxis = false;
    _fixedAxis = glm::vec3( 0, 1, 0 );

    position0 = GetCamera().Get_Position();
    target0 = GetCamera().Get_Look_At();
    up0 = GetCamera().Get_Up();
}

glm::vec2 Nova::TrackballCameraControl::getMouseOnCircle( const glm::vec2& pos ){
    glm::vec2 vector;
    vector.x = ( pos.x - screen.width * 0.5 - screen.left ) / ( screen.width * 0.5 );
    vector.y = ( ( screen.height + 2 * ( screen.top - pos.y ) ) / screen.width ); // screen.width intentional   
    return vector;
}


glm::vec2 Nova::TrackballCameraControl::getMouseOnScreen( const glm::vec2& pos ){    
    glm::vec2 vector;    
    vector.x = (pos.x - screen.left) / screen.width;
    vector.y = (pos.y - screen.top) / screen.height;;
    return vector;
}

void Nova::TrackballCameraControl::MouseDown(const Nova::IOEvent& event) {
    if ( enabled == false ) return;
    
    if ( _state == NONE ) {
        switch( event.mousebutton_data->button ){
        case Nova::IOEvent::M_LEFT:
            if( event.mousebutton_data->mods & IOEvent::mSHIFT ) return;
            _state = ROTATE;
            break;
        case Nova::IOEvent::M_RIGHT:
            _state = PAN;
            break;
        case Nova::IOEvent::M_MIDDLE:
            _state = ZOOM;
            break;
        }           
    }
    
    if ( _state == ROTATE && ! noRotate ) {
        _moveCurr = getMouseOnCircle( glm::vec2(event.mousebutton_data->x, event.mousebutton_data->y) );
        _movePrev = _moveCurr;
        
    } else if ( _state == ZOOM && ! noZoom ) {
        
        _zoomStart = getMouseOnScreen( glm::vec2(event.mousebutton_data->x, event.mousebutton_data->y) );
        _zoomEnd = _zoomStart;
        
    } else if ( _state == PAN && ! noPan ) {
        
        _panStart = getMouseOnScreen( glm::vec2(event.mousebutton_data->x, event.mousebutton_data->y) );
        _panEnd =  _panStart;
        
    }    
};

void Nova::TrackballCameraControl::MouseUp(const Nova::IOEvent& event) {
    if ( enabled == false ) return;
    _state = NONE;
};

void Nova::TrackballCameraControl::MouseMove(const Nova::IOEvent& event) {
    if ( enabled == false ) return;
    
    if ( _state == ROTATE && ! noRotate ) {
        _movePrev = _moveCurr;
        _moveCurr = getMouseOnCircle( glm::vec2(event.mousemotion_data->x, event.mousemotion_data->y) );
    } else if ( _state == ZOOM && ! noZoom ) {
        _zoomEnd = getMouseOnScreen( glm::vec2(event.mousemotion_data->x, event.mousemotion_data->y) );
        
    } else if ( _state == PAN && ! noPan ) {
        _panEnd = getMouseOnScreen( glm::vec2(event.mousemotion_data->x, event.mousemotion_data->y) );
    }
};

void Nova::TrackballCameraControl::KeyDown(const Nova::IOEvent& event) {
    if ( enabled == false ) return;        
    _prevState = _state;
    
    if ( _state != NONE ) {
        return;        
    } else if ( event.key_data->key == keys[ ROTATE ] && ! noRotate ) {
        _state = ROTATE;        
    } else if ( event.key_data->key == keys[ ZOOM ] && ! noZoom ) {
        _state = ZOOM;        
    } else if ( event.key_data->key == keys[ PAN ] && ! noPan ) {        
        _state = PAN;        
    }
};

void Nova::TrackballCameraControl::KeyUp(const Nova::IOEvent& event) {    
		if ( enabled == false ) return;        
		_state = _prevState;
};

void Nova::TrackballCameraControl::KeyHold(const Nova::IOEvent& event) {
    
};

void Nova::TrackballCameraControl::Redraw(const Nova::IOEvent& event) {
    screen.left = event.draw_data->x;
    screen.top = event.draw_data->y;
    screen.width = event.draw_data->width;
    screen.height = event.draw_data->height;
};


bool Nova::TrackballCameraControl::FixedAxis(glm::vec3& fixedAxis){
    if( _useFixedAxis ){
        fixedAxis = _fixedAxis;
        return true;
    }
    return false;              
}

void Nova::TrackballCameraControl::Scroll(const Nova::IOEvent& event) {
    if ( enabled == false ) return;   
    _zoomStart.y -= event.scroll_data->y * 0.01;
}

// State Methods
void Nova::TrackballCameraControl::Reset()
{
    _state = NONE;
    _prevState = NONE;

    target = target0;
    GetCamera().Set_Position( position0 );
    GetCamera().Set_Up( up0 );
    
    _eye = position0 - target;
    GetCamera().Set_Look_At( target );

    lastPosition = position0;
    GetCamera().Set_Mode(FREE);
};

void Nova::TrackballCameraControl::Update(const Nova::IOEvent& event) {
    
    _eye =  GetCamera().Get_Position() - target;
    
    if ( ! noRotate ) {       
        rotateCamera();        
    }
    
    if ( ! noZoom ) {
        zoomCamera();        
    }
    
    if ( ! noPan ) {        
        panCamera();        
    }
    
    GetCamera().Set_Position( target + _eye );

    checkDistances();
    
    GetCamera().Set_Look_At( target );

    
    if ( glm::distance2( lastPosition,  GetCamera().Get_Position() ) > EPS ) {
        lastPosition =  GetCamera().Get_Position();
    }

};

void Nova::TrackballCameraControl::rotateCamera(){

    glm::vec3 axis;
    glm::quat quaternion;
    glm::vec3 eyeDirection;
    glm::vec3 objectUpDirection;
    glm::vec3 objectSidewaysDirection;
    glm::vec3 moveDirection;
    float angle;
    
    float angleCurr = glm::atan( _moveCurr.x, _moveCurr.y );
    float anglePrev = glm::atan( _movePrev.x, _movePrev.y );
    float trueAngle;

    if( glm::sign( angleCurr ) == glm::sign( anglePrev ) )
        trueAngle = angleCurr - anglePrev;
    else{
        if( angleCurr > anglePrev )
            trueAngle = (angleCurr + -2*glm::pi<float>()) - anglePrev;
        else
            trueAngle = (angleCurr + 2*glm::pi<float>())  - anglePrev;
    }
        
    moveDirection = glm::vec3( _moveCurr.x - _movePrev.x, _moveCurr.y - _movePrev.y, 0 );
    angle = glm::length(moveDirection);

    if ( trueAngle ) {
        _eye = GetCamera().Get_Position() - target;
        eyeDirection = glm::normalize( _eye );

        if( _useFixedAxis ){
            quaternion = glm::angleAxis( -trueAngle, _fixedAxis );
            _eye = _eye * quaternion;
            GetCamera().Set_Up( GetCamera().Get_Up() * quaternion );
        }
        else{
        objectUpDirection =  glm::normalize(GetCamera().Get_Up());
        objectSidewaysDirection = glm::normalize( glm::cross( objectUpDirection, eyeDirection ) );

            objectUpDirection *= (-1*( _moveCurr.y - _movePrev.y ));
            objectSidewaysDirection *= (-1*( _moveCurr.x - _movePrev.x ));
        
        moveDirection = objectUpDirection + objectSidewaysDirection;

        axis = glm::normalize( glm::cross( moveDirection, _eye ) );
        angle *= rotateSpeed;
        quaternion = glm::angleAxis( angle, axis );

        _eye = _eye * quaternion;
        GetCamera().Set_Up( GetCamera().Get_Up() * quaternion );

        _lastAxis = axis;
        _lastAngle = angle;
        }


    } else if ( ! staticMoving && _lastAngle ) {
        
        _lastAngle *= sqrt( 1.0 - dynamicDampingFactor );
        _eye = GetCamera().Get_Position() - target;
        quaternion = glm::angleAxis( _lastAngle, _lastAxis );
        _eye = _eye * quaternion;
        GetCamera().Set_Up( GetCamera().Get_Up() * quaternion );       
    }

    _movePrev = _moveCurr;
}

void Nova::TrackballCameraControl::zoomCamera(){

		float factor;

        factor = 1.0 + ( _zoomEnd.y - _zoomStart.y ) * zoomSpeed;
        
        if ( factor != 1.0 && factor > 0.0 ) {
            _eye *= factor;
            
            if ( staticMoving ) {
                _zoomStart =  _zoomEnd;
            }
            else {
                _zoomStart.y += ( _zoomEnd.y - _zoomStart.y ) * dynamicDampingFactor;
            }            
        }        
}

void Nova::TrackballCameraControl::panCamera(){
    glm::vec2 mouseChange;
    glm::vec3 objectUp;
    glm::vec3 pan;

    mouseChange =  _panEnd - _panStart;
    
    if ( glm::length2(mouseChange) ) {
        mouseChange *= glm::length(_eye)* panSpeed;
        
        pan = ( glm::normalize(glm::cross( _eye , GetCamera().Get_Up())) * mouseChange.x);
        pan += (GetCamera().Get_Up()*mouseChange.y);
        
        GetCamera().Set_Position( GetCamera().Get_Position() + pan );
        target += pan;
        
        if ( staticMoving ) {            
            _panStart =  _panEnd ;            
        }
        else {
            _panStart += ((_panEnd - _panStart) * dynamicDampingFactor);
                }
        
    }    
}


void Nova::TrackballCameraControl::checkDistances(){

    if ( ! noZoom || ! noPan ) {
        
        if ( glm::length2(_eye) > maxDistance * maxDistance ) {            
            GetCamera().Set_Position( target + _eye * maxDistance );
            _zoomStart = _zoomEnd;            
        }
        
        if ( glm::length2(_eye) < minDistance * minDistance ) {            
            GetCamera().Set_Position( target + _eye * minDistance);
            _zoomStart =  _zoomEnd ;            
        }
        
    }
}
