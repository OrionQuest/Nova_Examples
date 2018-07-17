//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace Nova;
//#####################################################################
// Constructor
//#####################################################################
Camera::
Camera()
{
    camera_mode=FREE;
    field_of_view=45;
    rotation_quaternion=glm::quat(1,0,0,0);
    camera_position=glm::vec3(0,0,0);
    camera_position_delta=glm::vec3(0,0,0);
    camera_look_at=glm::vec3(0,0,1);
    camera_direction=glm::vec3(0,0,1);
    camera_up=glm::vec3(0,1,0);
    camera_scale=.5f;
    camera_pitch=0;
    camera_heading=0;
    camera_mouse_scale=.02f;
    max_pitch_rate=5;
    max_heading_rate=5;
    move_camera=false;
    viewport_x=0;
    viewport_y=0;
}
//#####################################################################
// Update
//#####################################################################
void Camera::
Update()
{
    camera_direction=glm::normalize(camera_look_at-camera_position);
    
    if(camera_mode==ORTHO){      
        
        projection=glm::ortho(-1.5f*float(aspect)*camera_scale,
                              1.5f*float(aspect)*camera_scale,
                              -1.5f*camera_scale,
                              1.5f*camera_scale,
                              (float)near_clip,(float)far_clip);
    }
    else if(camera_mode==FREE)
    {
        projection=glm::perspective(glm::radians(field_of_view),aspect,near_clip,far_clip);
        // detmine axis for pitch rotation
        //glm::vec3 axis=glm::cross(camera_direction,camera_up);
        // compute quaternion for pitch based on the camera pitch angle
        //glm::quat pitch_quat=glm::angleAxis(camera_pitch,axis);
        // determine heading quaternion from the camera up vector and the heading angle
        //glm::quat heading_quat=glm::angleAxis(camera_heading,camera_up);
        // add the two quaternions
        //glm::quat temp=glm::cross(pitch_quat,heading_quat);
        //temp=glm::normalize(temp);
        // update the direction from the quaternion
        //camera_direction=glm::rotate(temp,camera_direction);
        // add the camera delta
        //camera_position+=camera_position_delta;
        // set the look at to be infront of the camera
        //camera_look_at=camera_position+camera_direction*1.0f;
        // damping for smooth camera
        //camera_heading*=.5;
        //camera_pitch*=.5;
        //camera_position_delta=camera_position_delta*.5f;
    }

    // compute the MVP
    view=glm::lookAt(camera_position,camera_look_at,camera_up);
    model=glm::mat4(1.0f);
    MVP=projection*view*model;
}
//#####################################################################
// Move
//#####################################################################
void Camera::
Move(Camera_Direction dir)
{
    if(camera_mode==FREE)
    {
        switch(dir)
        {
            case UP:        camera_position_delta+=camera_up*camera_scale;
                            break;

            case DOWN:      camera_position_delta-=camera_up*camera_scale;
                            break;

            case LEFT:      camera_position_delta-=glm::cross(camera_direction,camera_up)*camera_scale;
                            break;

            case RIGHT:     camera_position_delta+=glm::cross(camera_direction,camera_up)*camera_scale;
                            break;

            case FORWARD:   camera_position_delta+=camera_direction*camera_scale;
                            break;

            case BACK:      camera_position_delta-=camera_direction*camera_scale;
                            break;
        }
    }
}
//#####################################################################
// Change_Pitch
//#####################################################################
void Camera::
Change_Pitch(float degrees)
{
    // check bounds with the max pitch rate so that we are not moving too fast
    if(degrees < -max_pitch_rate) degrees=-max_pitch_rate;
    else if(degrees > max_pitch_rate) degrees=max_pitch_rate;

    camera_pitch+=degrees*camera_mouse_scale;

    // check bounds for the camera pitch
    if(camera_pitch > 360.0f) camera_pitch-=360.0f;
    else if(camera_pitch < -360.0f) camera_pitch+=360.0f;
}
//#####################################################################
// Change_Heading
//#####################################################################
void Camera::
Change_Heading(float degrees)
{
    // check bounds with the max heading rate so that we are not moving too fast
    if(degrees < -max_heading_rate) degrees=-max_heading_rate;
    else if(degrees > max_heading_rate) degrees=max_heading_rate;

    // this controls how the heading changes if the camera is pointed straight up or down
    // the heading delta direction changes
    if((camera_pitch>90 && camera_pitch<270) || (camera_pitch<-90 && camera_pitch>-270))
        camera_heading -= degrees*camera_mouse_scale;
    else camera_heading += degrees*camera_mouse_scale;

    // check bounds for the camera heading
    if(camera_heading > 360.0f) camera_heading-=360.0f;
    else if(camera_heading < -360.0f) camera_heading+=360.0f;
}
//#####################################################################
// Move_2D
//#####################################################################
void Camera::
Move_2D(int x,int y)
{
    // compute the mouse delta from the previous mouse position
    glm::vec3 mouse_delta=mouse_position-glm::vec3(x,y,0);
    // if the camera is moving, meaning that the mouse was clicked and dragged,
    // change the pitch and heading
    if(move_camera)
    {
        Change_Heading(.08f*mouse_delta.x);
        Change_Pitch(.08f*mouse_delta.y);
    }
    mouse_position=glm::vec3(x,y,0);
}
//#####################################################################
// Set_Pos
//#####################################################################
void Camera::
Set_Pos(int button,int state,int x,int y)
{
    if(button==3 && state==GLFW_PRESS)
        camera_position_delta+=camera_up*.05f;
    else if(button==4 && state==GLFW_PRESS)
        camera_position_delta-=camera_up*.05f;
    else if(button==GLFW_MOUSE_BUTTON_LEFT && state==GLFW_PRESS)
        move_camera=true;
    else if(button==GLFW_MOUSE_BUTTON_LEFT && state==GLFW_RELEASE)
        move_camera=false;

    mouse_position=glm::vec3(x,y,0);
}
//#####################################################################
