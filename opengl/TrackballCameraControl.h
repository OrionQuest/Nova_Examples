//#####################################################################
// Copyright (c) 2016, Nathan Mitchell.
//#####################################################################
// Class TrackballCameraControl
//
//   Ported from the Trackball Camera Control from ThreeJS
//   https://github.com/mrdoob/three.js
//
//##################################################################### 
#ifndef __Trackball_Camera_Control__
#define __Trackball_Camera_Control__

#include "CameraControlInterface.h"
#include <glm/glm.hpp>
#include <array>

namespace Nova {

    class TrackballCameraControl  : public CameraControlBase {
        typedef CameraControlBase BASE;
        
    public:
        TrackballCameraControl( Camera& camera );

                // Input Events
        virtual void MouseDown(const IOEvent& event);
        virtual void MouseUp(const IOEvent& event);
        virtual void MouseMove(const IOEvent& event);
        virtual void KeyDown(const IOEvent& event);
        virtual void KeyUp(const IOEvent& event);
        virtual void KeyHold(const IOEvent& event);
        virtual void Redraw(const IOEvent& event);
        virtual void Scroll(const IOEvent& event);

        // State Methods
        virtual bool FixedAxis(glm::vec3& fixedAxis);
        virtual void Reset();
        virtual void Update(const IOEvent& event);


        // API
        bool enabled;
        struct Screen {
            int left;
            int top;
            int width;
            int height;
        } screen;
        
        float rotateSpeed;
        float zoomSpeed;
        float panSpeed;
        
        bool noRotate;
        bool noZoom;
        bool noPan;
        
        bool staticMoving;
        float dynamicDampingFactor;
        
        float minDistance;
        float maxDistance;
        
        std::array<Nova::IOEvent::KEY_CODE,3> keys;

    private:

        glm::vec2 getMouseOnCircle( const glm::vec2& pos );
        glm::vec2 getMouseOnScreen( const glm::vec2& pos );
        void rotateCamera();
        void zoomCamera();
        void panCamera();
        void checkDistances();

        enum STATE { NONE = -1, ROTATE =  0, ZOOM =  1, PAN =  2 };
        
        // internals
        
        glm::vec3 target;
        float EPS = 0.000001;
        
        glm::vec3 lastPosition;
        
        STATE _state;
        STATE _prevState;
            
        glm::vec3 _eye;
            
        glm::vec2 _movePrev;
        glm::vec2 _moveCurr;
            
        glm::vec3 _lastAxis;
        float _lastAngle;
            
        glm::vec2 _zoomStart;
        glm::vec2 _zoomEnd;
            
        glm::vec2 _panStart;
        glm::vec2 _panEnd;
        
        // for reset
        
        glm::vec3 target0;
        glm::vec3 position0;
        glm::vec3 up0;

        // Fixed Axis
        bool _useFixedAxis;
        glm::vec3 _fixedAxis;


    }; 

};

#endif
