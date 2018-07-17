//#####################################################################
// Copyright (c) 2016, Nathan Mitchell.
//#####################################################################
// Class PlanarCameraControl
//
//   Ported from the Planar Camera Control from ThreeJS
//   https://github.com/mrdoob/three.js
//
//##################################################################### 
#ifndef __Planar_Camera_Control__
#define __Planar_Camera_Control__

#include "CameraControlInterface.h"
#include <glm/glm.hpp>
#include <array>

namespace Nova {

    class PlanarCameraControl  : public CameraControlBase {
        typedef CameraControlBase BASE;
        
    public:
        PlanarCameraControl( Camera& camera );

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

        // "target" sets the location of focus, where the object planars around
        glm::vec3 target;
        glm::vec3 position;
        
        float minZoom;
        float maxZoom;
        float zoom;

     private:
        enum STATE { NONE = - 1, ROTATE = 0, DOLLY = 1, PAN = 2 };
        
        STATE state;
        float EPS;

        // for reset
        glm::vec3 target0;
        glm::vec3 position0;
        float zoom0;

        // Panning 
        glm::vec2 panStart;
        glm::vec2 panEnd;
        glm::vec2 panDelta;
        glm::vec3 panOffset;
        void pan( float deltaX, float deltaY );
        void panUp( float distance, glm::mat4 objectMatrix );
        void panLeft( float distance, glm::mat4 objectMatrix );

        // Zoom
        glm::vec2 dollyStart;
        glm::vec2 dollyEnd;
        glm::vec2 dollyDelta;
        bool zoomChanged;
        float zoomSpeed;        
        void dollyIn( float dollyScale );
        void dollyOut( float dollyScale );
        float getZoomScale();
        

        
    }; 

    
};

#endif
