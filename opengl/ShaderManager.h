#ifndef __OPENGL_3D_SHADER_MANAGER_H__
#define __OPENGL_3D_SHADER_MANAGER_H__

#include "Shader.h"
#include "ResourceManager.h"
#include <map>
#include <vector>
#include <string>
#include <memory>

namespace Nova {

    class ApplicationFactory;

    class ShaderManager : public AbstractResourceManager {
    public:
        ShaderManager(ApplicationFactory& app);
        virtual ~ShaderManager();
        
        std::unique_ptr<Shader> GetShader(const std::string& name);

    private:
        
        std::string ReadShader(std::string type, std::string path);
        unsigned int LoadFromFiles( std::string name );
        unsigned int LoadFromString( const char* vertex_shader,
                                     const char* fragment_shader,
                                     const char* geometry_shader );
        
        ApplicationFactory& _app;
        std::map<std::string, unsigned int> _shaderRepo;
    };
}

#endif
