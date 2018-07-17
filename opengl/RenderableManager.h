#ifndef __OPENGL_3D_RENDERABLE_MANAGER_H__
#define __OPENGL_3D_RENDERABLE_MANAGER_H__

#include <string>
#include <map>
#include <memory>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Nova {

    class ApplicationFactory;
    class Renderable;

    class Renderable {
    public:
    Renderable(ApplicationFactory& app) : _app(app) {};
      virtual ~Renderable() {};
      // Required Functions
      virtual void draw() = 0;
      
      // Optional Functions - Information
      virtual std::string name(){
          return std::string("UnnamedObject");          
      }

      virtual std::string render_class(){
          return std::string("BasicRenderable");          
      }

      // Optional Functions - Selection/Centering
      virtual glm::vec4 bounding_sphere() {
          return glm::vec4(0,0,0,-1);  // Return a -1 radius value to ignore sphere for centering
      };

      // Optional Functions - Selection
      virtual bool selectable() {return false;} ;
      virtual float hit_test( glm::vec3 start_point, glm::vec3 end_point ) { return 0.0f; };
      virtual void assign_selection( glm::vec3 start_point, glm::vec3 end_point, glm::vec3 intersection ) { };
      virtual void unassign_selection() { };

      // Optional Functions - SliceMode
      virtual bool slice_provider() {return false;};
      virtual glm::vec3 slice_start() {return glm::vec3(); };
      virtual float slice_dx() {return 0.0f; };
      virtual glm::vec3 max_slice() {return glm::vec3(0,0,0);};

    protected:
      ApplicationFactory& _app;
    };
    
    class RenderableFactory{
    public:
      RenderableFactory() {};
      virtual ~RenderableFactory() {};
      virtual std::string Name() {return "Unknown Factory";};
      
      virtual std::unique_ptr<Renderable> 
          Create(ApplicationFactory& app, std::string path ) = 0;
      virtual std::vector< std::unique_ptr<Renderable> > 
          CreateMultiple(ApplicationFactory& app, std::string path) 
      {return std::vector< std::unique_ptr<Renderable> >();};
      virtual bool AcceptExtension(std::string ext) const = 0;   
      virtual int NumRenderables(std::string path) { return 1; };      
    };
    
    class RenderableManager {
    public:
        RenderableManager( ApplicationFactory& app );
        virtual ~RenderableManager();

        std::vector<unsigned long> RegisterRenderables( std::string path );
        void AddFactory( std::unique_ptr<RenderableFactory> );
	  
        Renderable* Get(unsigned long id);
        unsigned long MaxId();
        
        int GetFactoryCount() const { return _factories.size(); };

    private:
        ApplicationFactory& _app;

        unsigned long _resource_counter;
        std::map< unsigned long, std::unique_ptr<Renderable> >  _resources;
	std::vector< std::unique_ptr<RenderableFactory> > _factories;
    };

}


#endif
