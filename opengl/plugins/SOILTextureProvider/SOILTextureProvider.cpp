#include "../../ApplicationFactory.h"
#include "../../TextureManager.h"

#include <string>
#include <iostream>
#include <SOIL/SOIL.h>


namespace Nova {

    // This is a simple TextureProvider implementation based on SOIL. 
    // While it works, it is rather inefficient, so it should mostly be
    // used as a reference.
    
    class SOILTextureProvider : public TextureProvider {
    public:
        SOILTextureProvider() : TextureProvider() {
            
        };

        virtual ~SOILTextureProvider() {

        };

        virtual bool TextureLoadable( std::string name) { 
            int width;
            int height;
            unsigned char* pixels = SOIL_load_image(name.c_str(),&width,&height,0,SOIL_LOAD_RGB);
            if( pixels == NULL )
                return false;
            else
                return true;            
        };

        virtual void TextureDimensions( std::string name,
                                        unsigned int& w,
                                        unsigned int& h,
                                        unsigned int& d,
                                        unsigned int& c) {
            int width, height;
            // The API requires that TextureLoadable was called first, so this should succeed... 
            unsigned char* pixels = SOIL_load_image(name.c_str(),&width,&height,0,SOIL_LOAD_RGB);
            w = width;
            h = height;
            d = 1; // SOIL doesn't handle 3D textures
            c = 3; // We are forcing RGB mode wo/Alpha           
        };

        virtual void TextureData( std::string name, unsigned char* pixels) {
            int width, height;

            // Since TextureLoadable and TextureDimensions are called first, we can
            // assume the texture will load and the pixels argument is of an approprate size
            unsigned char* SOILpixels = SOIL_load_image(name.c_str(),&width,&height,0,SOIL_LOAD_RGB);
            // The TextureManager owns these pixels, so we just copy into them and delete our own stuff
            memcpy( pixels, SOILpixels, width*height*3*sizeof(unsigned char) );            
            SOIL_free_image_data(SOILpixels);
        };
    };

}

extern "C" void registerPlugin(Nova::ApplicationFactory& app) {
  app.GetTextureManager().RegisterProvider( std::move( std::unique_ptr<Nova::TextureProvider>( new Nova::SOILTextureProvider() )));
}

extern "C" int getEngineVersion() {
  return Nova::API_VERSION;
}
