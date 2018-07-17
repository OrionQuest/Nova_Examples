#include "../../RenderableManager.h"
#include "../../ApplicationFactory.h"
#include "../../Shader.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Nova {

    class TextFileRenderer : public Renderable {        
        std::unique_ptr<ApplicationFactory> _app;
        std::vector<std::string> textLines;

    public:
        TextFileRenderer( ApplicationFactory& app ) : Renderable( app ), _app(std::unique_ptr<ApplicationFactory>(&app))
        {
            
        }

        virtual ~TextFileRenderer(){
            
        }
        
        virtual void load(std::string path){
            std::cout << "TextFileRenderer::load" << std::endl;            
            
            std::string content;
            // Get file at path 
            try {
                std::ifstream file(path);
                content.assign( 
                    ( std::istreambuf_iterator<char>(file) ),
                    ( std::istreambuf_iterator<char>() )
                    );
                textLines.push_back(content);
                file.close();
		// std::cout << "\n********************\nFile at \"" << path << "\" loaded successfully.\n";
		// std::cout << "\nNew file info:\n\"" << content << "\"\n********************\n\n";
            }
            catch(std::exception& e) {
                std::cout << "Exception occurred while reading text file. Error info: " << e.what() << std::endl;
                return; // Exception occured, just do nothing and return
            }

            
        }

        virtual void draw(){
            // TODO Replace hard coded variables //
            float scale = 1.0;
            float start_x = 10.0;
            float start_y = 20.0;
            float line_height = 20.0;
            ///////////////////////////////////////
            
            // Draw every line of text from this text file
            for (auto &text : textLines)
            {
                draw(text, scale, start_x, start_y);
                start_y += line_height; // Jump to next line on screen
            }
            
        }

	// Parse file and print contents of file to app
        virtual void draw(std::string text, float scale, float start_x, float start_y){
            // Print contents of file to app at specified position
            _app->GetTextRenderingService().Render( text, scale, start_x, start_y );
        }

    };

    class TextFileRendererFactory : public RenderableFactory {
  public:
    TextFileRendererFactory() : RenderableFactory()
    {}
    
    virtual ~TextFileRendererFactory() {}

    virtual std::unique_ptr<Renderable> Create( ApplicationFactory& app, std::string path ){
      TextFileRenderer* renderable = new TextFileRenderer(app);
      renderable->load( path );
      return std::unique_ptr<Renderable>( renderable );
    }
    
    virtual bool AcceptExtension(std::string ext) const {
      // TODO (Note : Returning true eventually causes seg fault at program termination/ closing)
      // Solve conflict that is causing ` key NOT to open in-app console anymore
      return (ext == "txt");
    };

  };

}

extern "C" void registerPlugin(Nova::ApplicationFactory& app) {
    app.GetRenderableManager().AddFactory( std::move( std::unique_ptr<Nova::RenderableFactory>( new Nova::TextFileRendererFactory() )));
}

extern "C" int getEngineVersion() {
    return Nova::API_VERSION;
}

