//#####################################################################
// Copyright (c) 2016, Nathan Mitchell.
//#####################################################################
// Class OrbitCameraControl
//
//   Ported from the Orbit Camera Control from ThreeJS
//   https://github.com/mrdoob/three.js
//
//##################################################################### 
#ifndef __Orbit_Camera_Control__
#define __Orbit_Camera_Control__

#include "CameraControlInterface.h"
#include <glm/glm.hpp>
#include <array>

namespace Nova {

    class OrbitCameraControl  : public CameraControlBase {
        typedef CameraControlBase BASE;
        
    public:
        OrbitCameraControl( Camera& camera );

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
        virtual void Reset();
        virtual void Update(const IOEvent& event);
        virtual bool HasFocus();

        // API
        
        // Set to false to disable this control
        bool enabled;

        struct Screen {
            int left;
            int top;
            int width;
            int height;
        } screen;

        // "target" sets the location of focus, where the object orbits around
        glm::vec3 target;

        // How far you can dolly in and out ( PerspectiveCamera only )
        float minDistance;
        float maxDistance;

        // How far you can zoom in and out ( OrthographicCamera only )
        float zoom;
        float minZoom;
        float maxZoom;

        // How far you can orbit vertically, upper and lower limits.
        // Range is 0 to Math.PI radians.
        float minPolarAngle;
        float maxPolarAngle;

        // How far you can orbit horizontally, upper and lower limits.
        // If set, must be a sub-interval of the interval [ - Math.PI, Math.PI ].
        float minAzimuthAngle;
        float maxAzimuthAngle;

        // Set to true to enable damping (inertia)
        // If damping is enabled, you must call controls.update() in your animation loop
        bool enableDamping;
        float dampingFactor;

        // This option actually enables dollying in and out; left as "zoom" for backwards compatibility.
        // Set to false to disable zooming
        bool enableZoom;
        float zoomSpeed;

        // Set to false to disable rotating
        bool enableRotate;
        float rotateSpeed;

        // Set to false to disable panning
        bool enablePan;
        float keyPanSpeed;

        // Set to true to automatically rotate around the target
        // If auto-rotate is enabled, you must call controls.update() in your animation loop
        bool autoRotate;
        float autoRotateSpeed;

        // Set to false to disable use of the keys
        bool enableKeys;

        // The four arrow keys
        std::array<Nova::IOEvent::KEY_CODE,4> keys;
        


    private:

        float getAutoRotationAngle();
        float getZoomScale();
        void rotateLeft( float angle );
        void rotateUp( float angle );
        void panLeft( float distance, glm::mat4 objectMatrix );
        void panUp( float distance, glm::mat4 objectMatrix );
        void pan( float deltaX, float deltaY );
        void dollyIn( float dollyScale );
        void dollyOut( float dollyScale );



        enum STATE { NONE = - 1, ROTATE = 0, DOLLY = 1, PAN = 2 };

        // internals        
        STATE state;
        
        float EPS;
        
        // current position in spherical coordinates
        glm::vec3 spherical;
        glm::vec3 sphericalDelta;
        
        float scale = 1;
        glm::vec3 panOffset;
        bool zoomChanged;
        
        glm::vec2 rotateStart;
        glm::vec2 rotateEnd;
        glm::vec2 rotateDelta;
        
        glm::vec2 panStart;
        glm::vec2 panEnd;
        glm::vec2 panDelta;

        glm::vec2 dollyStart;
        glm::vec2 dollyEnd;
        glm::vec2 dollyDelta;

        // for reset
        glm::vec3 target0;
        glm::vec3 position0;
        float zoom0;

    }; 

    
};

#endif
