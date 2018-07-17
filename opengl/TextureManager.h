#ifndef __OPENGL_3D_TEXTURE_MANAGER_H__
#define __OPENGL_3D_TEXTURE_MANAGER_H__

#include "ResourceManager.h"
#include <string>
#include <map>
#include <memory>

namespace Nova {

    class ApplicationFactory;

    class TextureProvider {
    public:
        TextureProvider() {};
        virtual ~TextureProvider() {};

        virtual bool TextureLoadable( std::string name) = 0;
        virtual void TextureDimensions( std::string name,
                                        unsigned int& width,
                                        unsigned int& height,
                                        unsigned int& depth,
                                        unsigned int& channels) = 0;
        virtual void TextureData( std::string name, unsigned char* pixels) = 0;

    private:

    };

    class TextureManager : public AbstractResourceManager {
    public:
        TextureManager( ApplicationFactory& app );
        virtual ~TextureManager();

        unsigned int GetOpenGLTexture( std::string name );
        void ReleaseOpenGLTexture( std::string name );
        void RegisterProvider( std::unique_ptr<TextureProvider> provider );
        bool HasProvider() { return (bool)(_textureProvider); };

    private:
        unsigned int GetMissingTexture();
        ApplicationFactory& _app;
        std::map< std::string, unsigned int> _textureRegistry;
        std::unique_ptr< TextureProvider > _textureProvider;
        unsigned int _missingTexture;
    };
    
}

#endif
