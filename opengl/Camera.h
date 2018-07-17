//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
// Class Camera
//##################################################################### 
#ifndef __Camera__
#define __Camera__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>

namespace Nova{

enum Camera_Type {ORTHO,FREE};
enum Camera_Direction {UP,DOWN,LEFT,RIGHT,FORWARD,BACK};

class Camera
{
  public:
    Camera_Type camera_mode;
    int viewport_x,viewport_y;
    int window_width,window_height;
    double aspect,field_of_view,near_clip,far_clip;
    float camera_scale,camera_heading,camera_pitch,camera_mouse_scale;
    float max_pitch_rate,max_heading_rate;
    bool move_camera;

    glm::vec3 camera_position,camera_position_delta,camera_look_at,camera_direction;
    glm::vec3 camera_up,mouse_position;
    glm::quat rotation_quaternion;

    glm::mat4 projection,view,model,MVP;

    Camera();
    ~Camera() {}

    inline void Reset()
    {camera_up=glm::vec3(0,1,0);}

    inline void Set_Mode(Camera_Type cam_mode)
    {
        camera_mode=cam_mode;
        camera_up=glm::vec3(0,1,0);
        rotation_quaternion=glm::quat(1,0,0,0);
    }

    inline glm::mat4 Get_ViewMatrix() const
    {return view;}
    inline glm::mat4 Get_ModelMatrix() const
    {return model;}
    inline glm::mat4 Get_ProjectionMatrix() const
    {return projection;}
    inline glm::mat4 Get_Matrix() const
    {return MVP;}

    inline float Get_Scale(){
        return camera_scale;
    };

    inline void Set_Scale(float scale){
        camera_scale = scale;
    };
    
    inline std::array<float,6> Get_Ortho_Box() const
    { return {-1.5f*float(aspect)*camera_scale,1.5f*float(aspect)*camera_scale,-1.5f*camera_scale,1.5f*camera_scale,(float)near_clip,(float)far_clip}; }
        
    inline void Set_Position(glm::vec3 pos)
    {camera_position=pos;}

    inline glm::vec3 Get_Position() const
    {return camera_position;}

    inline void Set_Up(glm::vec3 up)
    {camera_up=up;}

    inline glm::vec3 Get_Up() const
    {return camera_up;}

    inline void Set_Look_At(glm::vec3 look_at)
    {camera_look_at=look_at;}

    inline glm::vec3 Get_Look_At()
    {return camera_look_at;}

    inline glm::quat Get_Rotation() const
    {return rotation_quaternion;}

    inline void Set_FOV(double fov)
    {field_of_view=fov;}

    inline double Get_FOV() const 
    {return field_of_view;}

    inline void Set_Viewport(int loc_x,int loc_y,int width,int height)
    {
        viewport_x=loc_x;viewport_y=loc_y;
        window_width=width;window_height=height;
        aspect=(double)width/(double)height;
    }

    inline void Set_Clipping(double near_clip_distance,double far_clip_distance)
    {
        near_clip=near_clip_distance;
        far_clip=far_clip_distance;
    }

    inline Camera_Type Get_Mode() const
    {return camera_mode;}

    inline void Get_Viewport(int& loc_x,int& loc_y,int& width,int& height) const
    {
        loc_x=viewport_x;
        loc_y=viewport_y;
        width=window_width;
        height=window_height;
    }

    inline void Get_Matrices(glm::mat4& P,glm::mat4& V,glm::mat4& M) const
    {
        P=projection;
        V=view;
        M=model;
    }

//##################################################################### 
    void Update();
    void Move(Camera_Direction dir);
    void Change_Pitch(float degrees);
    void Change_Heading(float degrees);
    void Move_2D(int x,int y);
    void Set_Pos(int button,int state,int x,int y);
//##################################################################### 
};
}
#endif
