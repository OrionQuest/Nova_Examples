//#####################################################################
// Copyright (c) 2016, Nathan Mitchell.
//#####################################################################
// Class StaticCameraControl
//##################################################################### 
#ifndef __Static_Camera_Control__
#define __Static_Camera_Control__

#include "Camera.h"
#include "IOEvent.h"
#include "CameraControlInterface.h"
#include <iostream>

namespace Nova {

/*! class StaticCameraControl
  This is a dummy camera control that provides empty implementations of the CameraControlInterface
  , but also prints out events as they occur for debugging purposes.
*/
    class  StaticCameraControl : public CameraControlBase {
        typedef CameraControlBase BASE;

    public:
        StaticCameraControl( Camera& camera ) :  BASE( camera ) {}
        
        // Input Events
        virtual void MouseDown(const IOEvent& event) {
            std::cout << "MouseDown Event: " << 
                event.type << " " << 
                event.currentTime << " " <<
                event.mousebutton_data->button << " " <<
                event.mousebutton_data->button_raw << " " << 
                event.mousebutton_data->action << " " << 
                event.mousebutton_data->mods << std::endl;                
        };
        virtual void MouseUp(const IOEvent& event) {
            std::cout << "MouseUp Event: " << 
                event.type << " " << 
                event.currentTime << " " <<
                event.mousebutton_data->button << " " <<
                event.mousebutton_data->button_raw << " " << 
                event.mousebutton_data->action << " " << 
                event.mousebutton_data->mods << std::endl;                
        };
        virtual void MouseMove(const IOEvent& event) {
            std::cout << "MouseMove Event: " <<
                event.type << " " << 
                event.currentTime << " " <<
                event.mousemotion_data->x << " " << 
                event.mousemotion_data->y << std::endl;
        };

        virtual void KeyDown(const IOEvent& event) {
            std::cout << "KeyDown Event: " << 
                event.type << " " << 
                event.currentTime << " " <<
                event.key_data->key << " " << 
                event.key_data->scancode << " " << 
                event.key_data->action << " " <<
                event.key_data->mods << std::endl;
        };

        virtual  void KeyUp(const IOEvent& event) {
            std::cout << "KeyUp Event: " << 
                event.type << " " << 
                event.currentTime << " " <<
                event.key_data->key << " " << 
                event.key_data->scancode << " " << 
                event.key_data->action << " " <<
                event.key_data->mods << std::endl;
        };
        virtual void KeyHold(const IOEvent& event) {
            std::cout << "KeyHold Event: " << 
                event.type << " " << 
                event.currentTime << " " <<
                event.key_data->key << " " << 
                event.key_data->scancode << " " << 
                event.key_data->action << " " <<
                event.key_data->mods << std::endl;
        };

        virtual void Redraw(const IOEvent& event) {
            std::cout << "Redraw Event: "<< 
                event.type << " " <<
                event.currentTime << " " << 
                event.draw_data->width << " " <<
                event.draw_data->height << std::endl;
        };

        // State Methods
        virtual void Reset() {
            GetCamera().Set_Position(glm::vec3(0,0,0));
            GetCamera().Set_Look_At( glm::vec3(0,0,-1) );
            GetCamera().Set_Mode(FREE);
        };
        virtual void Update(const IOEvent& event) {
            std::cout << "Update: " << event.currentTime << std::endl;
        }

    };


}

#endif
