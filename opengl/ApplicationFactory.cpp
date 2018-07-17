#include "ApplicationFactory.h"
#include <iostream>
#include <memory>

Nova::ApplicationFactory::ApplicationFactory( const Nova::Config& config ) : _config( config )
{
    // This should be initialized first, in case other things want to initialize early bindings
    _ioservice = std::unique_ptr<IOService>( new IOService(*this) );
    _keybinder = std::unique_ptr<KeyBinder>( new KeyBinder(*this) );
    GetIOService().On( IOService::TIME, [&](IOEvent& event){_keybinder->Dispatch( event ); });
    GetIOService().On( IOService::REDRAW, [&](IOEvent& event){_keybinder->Dispatch( event ); });
    GetIOService().On( IOService::MOUSE_DOWN, [&](IOEvent& event){_keybinder->Dispatch( event ); });
    GetIOService().On( IOService::MOUSE_UP, [&](IOEvent& event){_keybinder->Dispatch( event ); });
    GetIOService().On( IOService::MOUSE_MOVE, [&](IOEvent& event){_keybinder->Dispatch( event ); });
    GetIOService().On( IOService::KEY_DOWN, [&](IOEvent& event){_keybinder->Dispatch( event ); });
    GetIOService().On( IOService::KEY_UP, [&](IOEvent& event){_keybinder->Dispatch( event ); });
    GetIOService().On( IOService::KEY_HOLD, [&](IOEvent& event){_keybinder->Dispatch( event ); });
    GetIOService().On( IOService::SCROLL, [&](IOEvent& event){_keybinder->Dispatch( event ); });
    GetIOService().On( IOService::DROP, [&](IOEvent& event){_keybinder->Dispatch( event ); });

    // The World is initialized second, as many things depend on the OpenGL context to be available;
    _world = std::unique_ptr<World>( new World(*this) );    
    _world->Initialize(800,600);

    _scene = std::unique_ptr<Scene>( new Scene(*this) );
    _shaderman = std::unique_ptr<ShaderManager>( new ShaderManager(*this) );
    _shaderman->AppendSearchPaths( _config.shaderpaths );
    _shaderman->PrependSearchPaths( _config.scenepath );
    _pluginman = std::unique_ptr<PluginManager>( new PluginManager(*this) );
    _pluginman->AppendSearchPaths( _config.pluginpaths );
    _pluginman->PrependSearchPaths( std::string("./") ); // Always search second in the current working directory
    _pluginman->PrependSearchPaths( _config.scenepath ); // Always search first in the scene directory
    _renderableman = std::unique_ptr<RenderableManager>( new RenderableManager(*this) );
    _textrenderingservice = std::unique_ptr<TextRenderingService>( new TextRenderingService(*this) );
    _commanddispatch = std::unique_ptr<CommandDispatch>( new CommandDispatch(*this) );
    _texturemanager = std::unique_ptr<TextureManager>( new TextureManager(*this) );
    _texturemanager->AppendSearchPaths( _config.texturepaths );
    _texturemanager->PrependSearchPaths( _config.scenepath ); // Always search first in the scene directory

    // Load all bindings from the config...
    std::cout << "Reading key bindings..." << std::endl;
    for( auto raw_binding : _config.bindings ){
        Binding binding;        
        //std::cout << "Reading Binding:  " << raw_binding << std::endl;
        bool res = _keybinder->Translate( raw_binding, binding );
        if( res ){
            try{
                _keybinder->Bind( binding );
            }
            catch( std::exception& e ){
                std::cout << "Binding:  " << raw_binding << std::endl;
                std::cout << e.what() << std::endl;
            }
        }        
        else{
            std::cout << "Binding:  " << raw_binding << std::endl;
            std::cout << "Could not bind. Inproperly formatted binding command." << std::endl;
        }
    }
}

Nova::IOService&
Nova::ApplicationFactory::GetIOService()
{ return *(_ioservice.get()); }

Nova::ShaderManager&
Nova::ApplicationFactory::GetShaderManager()
{ return *(_shaderman.get()); }

Nova::PluginManager&
Nova::ApplicationFactory::GetPluginManager()
{ return *(_pluginman.get()); }

Nova::RenderableManager&
Nova::ApplicationFactory::GetRenderableManager()
{ return *(_renderableman.get()); }

Nova::Scene&
Nova::ApplicationFactory::GetScene()
{ return *(_scene.get()); }

Nova::World&
Nova::ApplicationFactory::GetWorld()
{ return *(_world.get()); }

Nova::TextRenderingService&
Nova::ApplicationFactory::GetTextRenderingService()
{ return *(_textrenderingservice.get()); }

Nova::CommandDispatch&
Nova::ApplicationFactory::GetCommandDispatch()
{ return *(_commanddispatch.get()); }

Nova::KeyBinder&
Nova::ApplicationFactory::GetKeyBinder()
{ return *(_keybinder.get()); }

Nova::TextureManager&
Nova::ApplicationFactory::GetTextureManager()
{ return *(_texturemanager.get()); }

void
Nova::ApplicationFactory::RunSanityChecks() const {
    if( ! _textrenderingservice->HasProvider() ){
        std::cout << "Warning: No text rendering provider has been provided. Text rendering will be disabled." << std::endl;
    }
    if( _renderableman->GetFactoryCount() == 0 ) {
        std::cout << "Warning: No renderable factories have been registered. The scene won't be able to draw anything." << std::endl;
    }
}

void 
Nova::ApplicationFactory::Run()
{
    // Bind screen redraw events to text renderer to prevent distortion
    GetIOService().On( "WINDOW-RESIZE", [&](IOEvent& event){_textrenderingservice->UpdateScreenSize( event ); });

    // Bind the keydown and time events for the command dispatcher to read user input and render text
    GetIOService().On( "ACTIVATE-CONSOLE", [&](IOEvent& event){ _commanddispatch->Input( event ); });
    GetIOService().On( "RENDER-FRAME", [&](IOEvent& event){  _commanddispatch->Render( event ); });

    // Bind the mouse down to the scene to allow it to process selection attempts
    GetIOService().On( "START-SELECTION", [&](IOEvent& event){ _scene->Selection( event ); });

    // Animation 
    GetIOService().On( "PLAY-PAUSE", [&](IOEvent& event){ _scene->TogglePlayback(); });
    GetIOService().On( "RESET-FRAME", [&](IOEvent& event){ _scene->StopPlayback(); });

    std::cout << "Loading scene from '" << _config.scenepath << "'" << std::endl; 
    _scene->Configure( _config.scenepath );

    RunSanityChecks();

    _scene->Load();
    _world->Main_Loop();
}
