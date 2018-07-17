#ifndef __OPENGL_3D_VIEWPORT_MANAGER_H__
#define __OPENGL_3D_VIEWPORT_MANAGER_H__

#include <string>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <map>
#include <array>

namespace Nova {
    
    class Camera;
    class CameraControlBase;
    class ApplicationFactory;
    class IOEvent;

    class CameraFactory {
    public:
        CameraFactory(){};
        virtual ~CameraFactory(){};        
        virtual std::unique_ptr<CameraControlBase> BuildCameraController(Camera& camera) = 0;
    };
    
    template<class ControllerType>
    class SimpleCameraFactory : public CameraFactory{
    public:
        SimpleCameraFactory() : CameraFactory() {};
        virtual ~SimpleCameraFactory() {};        
        virtual std::unique_ptr<CameraControlBase> BuildCameraController(Camera& camera) {
            return std::move( std::unique_ptr<CameraControlBase>( new ControllerType(camera) ));
        };
    };

    class ViewportManager {
    public:
        enum ViewportConfiguration { VM_NOT_CONFIGURED, VM_SINGLE_VIEWPORT, VM_DUAL_VIEWPORT};//, VM_QUAD_VIEWPORT };
        enum RenderStyles{ RS_NOSTYLE=0, RS_WIREFRAME=1, RS_SOLID=2, RS_LIGHTING=4, RS_SLICE=8 };
        ViewportManager( ApplicationFactory& app );
        virtual ~ViewportManager();

        // These methods manage the global viewport state
        unsigned int NumViewports();
        unsigned int CurrentViewport();
        void SetWindowGeometry( int width, int height );
        void SetGlobalViewport();
        void SetViewport(unsigned int viewport);
        void ConfigureViewports(ViewportConfiguration wc);
        std::vector<std::string> AvailableControllers();
        void ResetAll(); // Reset all controllers for all viewports
        void HandleEvent( IOEvent& event );
        void Set_Controller( std::string controller, unsigned int viewport );
        void InitializeChrome();
        void DrawFrame();
        void DrawFrustrums();
        void Set_ClipBounds( float near, float far, unsigned int viewport );

        bool IsPrimaryViewport() const;
        unsigned int GlobalViewportId() const ;
        unsigned int ActiveViewportId() const ;
        unsigned int InteractionViewportId() const ;
        unsigned int FocusedViewportId() const ;
        
        void Configure_SliceMode( glm::vec3 start, float dx, glm::vec3 max_slice );
        void Toggle_SliceMode( unsigned int viewport );
        void Change_Slice( unsigned int viewport, int slice_offset );
        void Change_SliceAxis( unsigned int viewport );

        uint32_t RenderStyle(uint32_t mask, unsigned int v) const;
        std::array<glm::vec4, 2> GetSlicePlanes(unsigned int v) const;

        // These methods all act upon the CurrentViewport();
        void Update();
        uint32_t RenderStyle(uint32_t mask=0xFFFFFFFF) const;
        glm::vec3 GetCameraPosition() const;
        std::array<glm::vec4, 2> GetSlicePlanes() const;
        glm::mat4 GetViewMatrix() const; 
        glm::mat4 GetModelMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        glm::vec4 GetViewport() const;
        void DrawAxis();

        // These methods act upon the interacted/focused viewport
        glm::mat4 GetInteractionViewMatrix() const;
        glm::mat4 GetInteractionModelMatrix() const;
        glm::mat4 GetInteractionProjectionMatrix() const;
        glm::vec4 GetInteractionViewport() const;

    private:
        const unsigned int GLOBAL_VIEWPORT_ID=0xFFFFFFFF;
        enum AxisMode { AXIS_DISABLE=0, AXIS_CENTER=1, AXIS_CORNER=2, AXIS_MAX_VALUE=3 };
        enum SliceAxis { SLICE_X=0, SLICE_Y=1, SLICE_Z=2, MAX_AXIS=3 };
        const float DEFAULT_CLIP_NEAR;
        const float DEFAULT_CLIP_FAR;

        struct SliceInfo {
            SliceAxis axis;
            glm::vec3 slice_start;
            glm::vec3 max_slice;
            float slice_dx;
            int slice_inc;
        };

        struct Viewport {
            Viewport() = default;
            Viewport(Viewport&&) = default;
            Viewport(const Viewport&) = delete;
            Viewport& operator=(const Viewport&) = delete;
            int x;
            int y;
            int width;
            int height;
            float clipNear;
            float clipFar;
            std::unique_ptr<Camera> camera;
            std::unique_ptr<CameraControlBase> controller;            
            SliceInfo slice_info;

            // Contains flags indicating various render modes are enabled/disabled
            uint32_t render_style;
        };

        void UpdateViewportGeometry( unsigned int viewport );
        
        bool sliceModeConfigured;
        SliceInfo referenceSliceInfo;

        bool _debug_frustrum;
        unsigned int chromeVAO, chromeVBO;
        unsigned int axisVAO, axisVBO;
        unsigned int frustrumVAO, frustrumVBO, frustrumEBO;
        ViewportConfiguration _currentConfiguration;
        glm::vec2 _geometry;
        std::map< std::string, std::unique_ptr<CameraFactory> > _controllerFactories; 
        std::vector< Viewport > _viewports;
        Viewport _global_viewport;
        unsigned int _activeViewport;
        unsigned int _recentlyInteractedViewport;
        bool _focused;
        unsigned int _focusedViewport;
        unsigned int axis_mode;
        
        ApplicationFactory& _app;

    };

}

#endif
