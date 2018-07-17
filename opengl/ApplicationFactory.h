#ifndef __OPENGL_3D_APPLICATION_FACTORY_H__
#define __OPENGL_3D_APPLICATION_FACTORY_H__

#include "Config.h"
#include <memory>
#include <ostream>
#include <streambuf>

#include "ShaderManager.h"
#include "Scene.h"
#include "PluginManager.h"
#include "RenderableManager.h"
#include "World.h"
#include "IOService.h"
#include "TextRenderingService.h"
#include "CommandDispatch.h"
#include "TextureManager.h"

namespace Nova{


    constexpr int API_VERSION = 1;
  
    class ApplicationFactory {
        public:
            ApplicationFactory( const Config& config );
            void Run();

            const Config& GetConfig(){return _config;};
            IOService& GetIOService();
            ShaderManager& GetShaderManager();
            PluginManager& GetPluginManager();
            RenderableManager& GetRenderableManager();
            Scene&         GetScene();
            World&         GetWorld();
            TextRenderingService& GetTextRenderingService();
            CommandDispatch& GetCommandDispatch();
            KeyBinder& GetKeyBinder();
            TextureManager& GetTextureManager();

        private:        
            void RunSanityChecks() const;

            Config _config;
            std::unique_ptr<IOService> _ioservice;
            std::unique_ptr<World> _world;
            std::unique_ptr<Scene> _scene;
            std::unique_ptr<ShaderManager> _shaderman;
            std::unique_ptr<PluginManager> _pluginman;
            std::unique_ptr<RenderableManager> _renderableman;
            std::unique_ptr<TextRenderingService> _textrenderingservice;
            std::unique_ptr<CommandDispatch> _commanddispatch;
            std::unique_ptr<KeyBinder> _keybinder;
            std::unique_ptr<TextureManager> _texturemanager;
    };
}

#endif
