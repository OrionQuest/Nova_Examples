#include "../../ApplicationFactory.h"
#include "../../TextureManager.h"

#include <string>
#include <iostream>

#include <OpenImageIO/imagebuf.h>

namespace Nova {

    // This is a TextureProvider implementation based on OIIO. 
    
    class OIIOTextureProvider : public TextureProvider {
        OIIO::ImageCache* cache;

    public:
        OIIOTextureProvider() : TextureProvider(), cache(NULL) {
            cache = OIIO::ImageCache::create (false);
            cache->attribute ("unassociatedalpha", 1);
        };

        virtual ~OIIOTextureProvider() {
            OIIO::ImageCache::destroy (cache);
        };

        virtual bool TextureLoadable( std::string name) { 
            OIIO::ImageSpec spec;
            bool exists = cache->get_imagespec ( OIIO::ustring(name) , spec );
            return exists;
        };

        virtual void TextureDimensions( std::string name,
                                        unsigned int& w,
                                        unsigned int& h,
                                        unsigned int& d,
                                        unsigned int& c) {
            OIIO::ImageBuf buf( name, 0, 0, cache );
            const OIIO::ImageSpec& spec = buf.spec();
            w = spec.width;
            h = spec.height;
            d = spec.depth;
            c = spec.nchannels;
            return;
        };

        virtual void TextureData( std::string name, unsigned char* pixels) {
            OIIO::ImageBuf buf( name, 0, 0, cache );
            const OIIO::ImageSpec& spec = buf.spec();
            int nc = buf.nchannels();            
            // Iterate over all pixels of the image

            for (OIIO::ImageBuf::ConstIterator<unsigned char,unsigned char> it (buf); ! it.done(); ++it){
                for (int c = 0; c < nc; ++c){
                    unsigned int location = ((((spec.height-it.y()-1) // Flipped y-axis  
                                               *spec.width+it.x())  // Times width, add width offset
                                              *spec.depth+it.z()) // Times depth, add depth offset
                                             *nc + c);              // Times num-channels, add channel offset
                    pixels[ location ] = it[c];
                }
            }
            return;
        };
    };

}

extern "C" void registerPlugin(Nova::ApplicationFactory& app) {
  app.GetTextureManager().RegisterProvider( std::move( std::unique_ptr<Nova::TextureProvider>( new Nova::OIIOTextureProvider() )));
}

extern "C" int getEngineVersion() {
  return Nova::API_VERSION;
}
