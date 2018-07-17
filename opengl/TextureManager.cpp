#include "TextureManager.h"
#include "ApplicationFactory.h"
#include <iostream>

Nova::TextureManager::TextureManager(ApplicationFactory& app) : _app(app), _missingTexture(0) {
}

Nova::TextureManager::~TextureManager() {
    for( auto& tex : _textureRegistry ){
        glDeleteTextures( 1, &tex.second );
    }    
    _textureRegistry.clear();
    if( _missingTexture ){
        glDeleteTextures(1, &_missingTexture );
        _missingTexture = 0;
    }
}

unsigned int
Nova::TextureManager::GetOpenGLTexture( std::string name ){
    auto res = _textureRegistry.find( name );
    if( res != _textureRegistry.end() )
        return res->second;
    else{
        if( _textureProvider ){
            std::vector< std::string > potential_paths = GetAllPaths( name );
            for( auto potential_path : potential_paths ){
                if( _textureProvider->TextureLoadable( potential_path ) ){                
                    unsigned int width, height, depth, channels;
                    _textureProvider->TextureDimensions( potential_path, width, height, depth, channels );
                    if( depth != 1 ){
                        std::cout << name << ": Texture malformed. Depth is not one." << std::endl;
                        return GetMissingTexture();
                    }
                    if( channels < 1 || channels > 4 ){
                        std::cout << name << ": Texture malformed. Num. channels not between 1 and 4." << std::endl;
                        return GetMissingTexture();
                    }
                    
                    unsigned char* pixels = new unsigned char[ width*height*depth*channels ];
                    _textureProvider->TextureData( potential_path, pixels );
                    
                    GLuint texture_id;
                    glGenTextures(1,&texture_id);
                    glBindTexture(GL_TEXTURE_2D,texture_id);
                    switch( channels ){
                    case 1:
                        glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width,height,0, GL_RED, GL_UNSIGNED_BYTE, pixels);
                        break;
                    case 2:
                        glTexImage2D(GL_TEXTURE_2D,0,GL_RG,width,height,0, GL_RG, GL_UNSIGNED_BYTE, pixels);
                        break;
                    case 3:
                        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
                        break;
                    case 4:
                        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                        break;
                    }
                    glGenerateMipmap(GL_TEXTURE_2D);
                    
                    //TODO: Do we want to control these parameters from above ???
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                    
                    glBindTexture(GL_TEXTURE_2D,0);              
                    delete [] pixels;
                    
                    _textureRegistry.insert( std::make_pair( name, texture_id ) );
                    return texture_id;
                }
                else{
                    std::cout << name << ": Texture reported as not loadable." << std::endl;
                    return GetMissingTexture();
                }
            }
            std::cout << name << ": No path for this texture could be found." << std::endl;
            return GetMissingTexture();
        }
        else {
            std::cout << name << ": No Texture provider available." << std::endl;
            return GetMissingTexture();
        }        
    }
}


unsigned int
Nova::TextureManager::GetMissingTexture(){
    if(! _missingTexture ) {
    // Need to generate a new missing texture buffer

        GLuint texture_id;
        glGenTextures(1,&texture_id);
        glBindTexture(GL_TEXTURE_2D,texture_id);
        int width = 256;
        int height = 256;
        unsigned char* pixels = new unsigned char[ width*height ];
        memset( pixels, 0,  width*height*sizeof(unsigned char) );

        for( int i = 0; i < width; i++ )
            for( int j = 0; j < height; j++ ){
                bool iswhite = ((i % 32) / 16) ^ ((j % 32) / 16);
                if( iswhite )
                    pixels[i*width + j] = 255;
            }

        glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width,height,0, GL_RED, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D,0);              
        delete [] pixels;

        _missingTexture = texture_id;
    }
    return _missingTexture;
}

void
Nova::TextureManager::ReleaseOpenGLTexture( std::string name ){
    auto res = _textureRegistry.find( name );
    if( res != _textureRegistry.end() ){
        glDeleteTextures(1, &res->second );
        _textureRegistry.erase( res );
    }
}

void
Nova::TextureManager::RegisterProvider( std::unique_ptr< Nova::TextureProvider > provider ){
    _textureProvider = std::move( provider );
}


