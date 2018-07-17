#ifndef __OPENGL_3D_TEXT_RENDERING_SERVICE_H__
#define __OPENGL_3D_TEXT_RENDERING_SERVICE_H__

#include <string>
#include <memory>
#include <ostream>
#include <streambuf>

namespace Nova {

    class ApplicationFactory;
    class IOEvent;
    
    class TextRenderer{
    public:
        TextRenderer() { };
        virtual ~TextRenderer() { };

        virtual void RenderText( std::string text, float scaling, float x, float y, float width, float height) = 0;
    };
    
    class TextRenderingService : public std::ostream, public std::streambuf {

    public:
        TextRenderingService( ApplicationFactory& app);
        virtual ~TextRenderingService();

        void UpdateScreenSize(const IOEvent& event);
        void RegisterProvider( std::unique_ptr< TextRenderer > provider ); 
        void Flush();
        void Render(  std::string text, float scaling, float x, float y );
        bool HasProvider() const {return (bool)(_provider);};
        
        // Overloaded functions for stream operations
        virtual int overflow(int c);

    private:
        std::string buffer;
        int width;
        int height;
        ApplicationFactory& _app;
        std::unique_ptr< TextRenderer > _provider;
    };
}


#endif
