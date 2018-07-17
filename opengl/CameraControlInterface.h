//#####################################################################
// Copyright (c) 2016, Nathan Mitchell.
//#####################################################################
// Class OpenGL_CameraControlBase
//##################################################################### 
#ifndef __OpenGL_Camera_Control_Interface__
#define __OpenGL_Camera_Control_Interface__

#include "Camera.h"
#include "IOEvent.h"

namespace Nova {

    class CameraControlBase {
        
    public:
        CameraControlBase( Camera& camera ) : _camera(camera) {}
        
        // Input Events
        virtual void MouseDown(const IOEvent& event) {};
        virtual void MouseUp(const IOEvent& event) {};
        virtual void MouseMove(const IOEvent& event) {};
        virtual void KeyDown(const IOEvent& event) {};
        virtual void KeyUp(const IOEvent& event) {} ;
        virtual void KeyHold(const IOEvent& event) {};
        virtual void Redraw(const IOEvent& event) {} ;
        virtual void Scroll(const IOEvent& event) {} ;

        // State Methods
        virtual bool FixedAxis(glm::vec3& fixedAxis) {return false;};
        virtual bool HasFocus() {return false;} ;
        virtual void Reset() = 0;
        virtual void Update( const IOEvent& event ) {};
        Camera& GetCamera() { return _camera; };


    private:
        Camera& _camera;

    };

}

#endif
