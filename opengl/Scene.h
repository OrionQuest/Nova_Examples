#ifndef __OPENGL_3D_SCENE_H__
#define __OPENGL_3D_SCENE_H__

#include <string>
#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Nova{
    
    class ApplicationFactory;
    class IOEvent;

    class Scene {
    public:
        Scene(ApplicationFactory& app);
        void Configure(std::string path);
        void Load();
        void Selection( IOEvent& event );
        void Draw(); // OpenGL calls go here!
        void Playback( IOEvent& event);
        void TogglePlayback();
        void StopPlayback();

        // Informational / Helper functions
        glm::vec4 BoundingSphere();

    private:
        void InitializeSliceParameters();
        void BuildBoundingSphere();
        
        bool _debugHitSelection;
        glm::vec3 hit_start;
        glm::vec3 hit_end;
        unsigned long selected_object;
        ApplicationFactory& _app;
        std::string _path;
        glm::vec4 scene_sphere;

        unsigned int playback_callback;
        bool playback_enabled;
        double animation_fps;

        struct SceneObject {
            std::string src_path;
            unsigned long renderable_id;
            bool enable_draw;
        };

        std::vector< std::unique_ptr<SceneObject> > _scene_objects;
    };

}


#endif
