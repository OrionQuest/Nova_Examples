//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
// Class World
//##################################################################### 
#ifndef __World__
#define __World__

//#include "Camera.h"
//#include "CameraControlInterface.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <array>
#include <vector>

namespace Nova{
    class ApplicationFactory;
    class Scene;
    class ViewportManager;    

    class World
    {
        GLFWwindow *window;
        
        ApplicationFactory& _app;        
        glm::ivec2 window_size,window_position,window_physical_size;
        double dpi;
        float window_aspect;
        std::unique_ptr<ViewportManager> viewport;
        
    public:
        World(ApplicationFactory& app);
        ~World();
        void Initialize_Camera_Controls();
        
        enum MATRIX_MODE { DRAW, INTERACTION };
        
        glm::mat4 Get_ViewMatrix(MATRIX_MODE mode = DRAW) const ;
        glm::mat4 Get_ModelMatrix(MATRIX_MODE mode = DRAW) const ;
        glm::mat4 Get_ProjectionMatrix(MATRIX_MODE mode = DRAW) const ; 
        glm::vec4 Get_Viewport(MATRIX_MODE mode = DRAW) const;
        
        std::array< glm::vec4, 2 > Get_Slice_Planes() const;
        std::array< glm::vec4, 2 > Get_Slice_Planes(unsigned int v) const;

        // State Query Methods
        bool SliceMode() const;
        bool SliceMode(unsigned int v) const;
        bool WireframeMode() const;
        bool WireframeMode(unsigned int v) const;
        bool SolidMode() const;
        bool SolidMode(unsigned int v) const;
        bool LightingMode() const;
        bool LightingMode(unsigned int v) const;
        
        // Lighting Information
        std::vector< glm::vec3 > GetSceneLightPositions() const;
        glm::vec3 GetCameraPosition() const;

        // Window Information
        glm::ivec2 WindowSize() const;
        glm::ivec2 WindowPosition() const;
        glm::ivec2 ScreenSize() const;
        float Get_DPI() const;

        ViewportManager& GetViewportManager();

        static void Close_Callback(GLFWwindow* window);
        static void Scroll_Callback( GLFWwindow* window,double xoffset,double yoffset);              
        static void Reshape_Callback(GLFWwindow* window,int w,int h);                
        static void Keyboard_Callback(GLFWwindow* window,int key,int scancode,int action,int mode);              
        static void Mouse_Button_Callback(GLFWwindow* window,int button,int state,int mods);                
        static void Mouse_Position_Callback(GLFWwindow* window,double x,double y);
        
        
//##################################################################### 
        void Initialize(int size_x,int size_y);
        void Handle_Input(int key);
        void Main_Loop();
//##################################################################### 
    };
}
#endif
