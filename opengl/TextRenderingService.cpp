#include "TextRenderingService.h"
#include "IOEvent.h"
#include "ApplicationFactory.h"
#include <iostream>

Nova::TextRenderingService::TextRenderingService(Nova::ApplicationFactory& app) : _app(app), std::ostream( this )
{
    width = _app.GetWorld().WindowSize().x;
    height = _app.GetWorld().WindowSize().y;
}

Nova::TextRenderingService::~TextRenderingService()
{

}

void
Nova::TextRenderingService::RegisterProvider(  std::unique_ptr< TextRenderer > provider )
{
    _provider = std::move( provider );
}

void
Nova::TextRenderingService::Flush(){
    Render(buffer, 1, 0, 0);
    buffer = "";
}

void
Nova::TextRenderingService::Render(  std::string text, float scaling, float x, float y )
{
    glm::vec4 viewport = _app.GetWorld().Get_Viewport();
    width = viewport.z;
    height = viewport.w;
    if( _provider )
        _provider->RenderText( text, scaling, x, y, width, height );
}


void
Nova::TextRenderingService::UpdateScreenSize(const Nova::IOEvent& event){
    width = event.draw_data->width;
    height = event.draw_data->height;
}

int
Nova::TextRenderingService::overflow( int c ){
    buffer += c;
    return 0;
}
