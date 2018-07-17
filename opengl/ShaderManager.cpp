#include "ShaderManager.h"

// Include all Builtin Shaders //
#include "shaders/BasicMeshShader.h"
#include "shaders/BasicLineShader.h"
#include "shaders/BasicTextShader.h"
#include "shaders/ViewportChromeShader.h"
#include "shaders/ViewportAxisShader.h"
#include "shaders/BasicColored.h"

// Addon for Slice Mode
#include "shaders/SliceModeAddon.vert_mod"


// Done                        //

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

Nova::ShaderManager::ShaderManager(ApplicationFactory& app) : _app(app)
{
    // Need to populate the Built-in Shader maps. 
    // These shaders are compiled in and never change

#define LOAD_BUILTIN_SHADER( NAME )  {\
        unsigned int id = LoadFromString(NovaBuiltinShaders::NAME::vertex_shader,\
                                         NovaBuiltinShaders::NAME::fragment_shader,\
                                         NovaBuiltinShaders::NAME::geometry_shader);\
        assert( id );\
        std::string name( NovaBuiltinShaders::NAME::name );\
        _shaderRepo.insert( std::make_pair( name , id ) );\
    }

    LOAD_BUILTIN_SHADER( BasicMeshShader );
    LOAD_BUILTIN_SHADER( BasicLineShader );
    LOAD_BUILTIN_SHADER( BasicTextShader );
    LOAD_BUILTIN_SHADER( ViewportChromeShader );
    LOAD_BUILTIN_SHADER( ViewportAxisShader );
    LOAD_BUILTIN_SHADER( BasicColored );

#undef LOAD_BUILTIN_SHADER

    
}

Nova::ShaderManager::~ShaderManager() 
{
    for( auto shader : _shaderRepo ){
        glDeleteProgram( shader.second );
    }
}

std::unique_ptr<Nova::Shader>
Nova::ShaderManager::GetShader(const std::string& name)
{
    auto res = _shaderRepo.find( name );
    if( res == _shaderRepo.end() ){
        unsigned int program_id = LoadFromFiles( name );        
        return std::unique_ptr<Nova::Shader>( new Shader(program_id) );           
    }
    else{
        return std::unique_ptr<Nova::Shader>( new Shader(res->second) );           
    }
}

std::string
Nova::ShaderManager::ReadShader( std::string type, std::string path ){
    std::string code = "";
    if( path != "" ){
        std::ifstream shader_file;
        std::stringstream shader_stream;
        shader_file.exceptions(std::ifstream::badbit);
        try{
            // open files
            shader_file.open(path.c_str());       
            // read file's buffer contents into streams
            shader_stream<<shader_file.rdbuf();
            // close file handlers
            shader_file.close();
            // convert stream into string
            code=shader_stream.str();
        }
        catch(std::ifstream::failure& e){
            std::cout << "Shader: "+type+" Program Not Successfully Read!" << std::endl;        
        }
    }
    else
        std::cout << "Shader: "+type+" Program Not Found!" << std::endl;

    return code;
}

unsigned int 
Nova::ShaderManager::LoadFromFiles( std::string name )
{
    // We need to search shader searchpaths for all files with the patterns:
    //      path/name.vert
    //      path/name.frag
    //      path/name.geom
    // And load them into a program

    // Path construction
    std::string vertex_filename, fragment_filename, geometry_filename;
    vertex_filename = name + ".vert";
    fragment_filename = name + ".frag";
    geometry_filename = name + ".geom";

    // retrieve the vertex/fragment source code
    std::string vertex_code,fragment_code,geometry_code;
    std::string vertex_shader_path = GetPath( vertex_filename );
    std::string fragment_shader_path = GetPath( fragment_filename );
    std::string geometry_shader_path = GetPath( geometry_filename );

    vertex_code = ReadShader( "Vertex", vertex_shader_path );
    fragment_code = ReadShader( "Fragment", fragment_shader_path );
    geometry_code = ReadShader( "Geometry", geometry_shader_path );
    
    if( vertex_code == "" || fragment_code == "" )
        throw std::runtime_error( "Could not load shader. Both vertex and fragment code must be present." );

    unsigned int program_id = 0;
    if( geometry_code != "" )
        program_id = LoadFromString( vertex_code.c_str(), fragment_code.c_str(), geometry_code.c_str() );
    else
        program_id = LoadFromString( vertex_code.c_str(), fragment_code.c_str(), NULL );

    if( program_id == 0 )
        throw std::runtime_error( std::string("Failed to compile and link shader: ") + name );
    
    _shaderRepo.insert( std::make_pair( name, program_id ) );

    return program_id;
}

unsigned int 
Nova::ShaderManager::LoadFromString( const char* vertex_shader,
                                     const char* fragment_shader,
                                     const char* geometry_shader )
{

    // compile shaders
    GLuint vertex,fragment,geometry;
    GLuint program;
    GLint success;
    GLchar info_log[512];

    const char* common_header = R"lang::GLSL(
#version 330 core
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
)lang::GLSL";
   

    // vertex shader
    vertex=glCreateShader(GL_VERTEX_SHADER);
    const char* vertex_source[3];
    vertex_source[0] = common_header;
    vertex_source[1] = NovaBuiltinShaders::SliceModeAddon::vertex_shader_addon;
    vertex_source[2] = vertex_shader;
    glShaderSource(vertex,3,vertex_source,NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(vertex,512,NULL,info_log);
        std::cout<<"Error::Shader::Vertex::Compilation Failed!"<<std::endl<<info_log<<std::endl;
        return 0;
    }

    // fragment shader
    fragment=glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragment_source[2];
    fragment_source[0] = common_header;
    fragment_source[1] = fragment_shader;
    glShaderSource(fragment,2,fragment_source,NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(fragment,512,NULL,info_log);
        std::cout<<"Error::Shader::Fragment::Compilation Failed!"<<std::endl<<info_log<<std::endl;
        return 0;
    }

    if( geometry_shader ){
        // geometry shader
        geometry=glCreateShader(GL_GEOMETRY_SHADER);
        const char* geometry_source[3];
        geometry_source[0] = common_header;
        geometry_source[1] = NovaBuiltinShaders::SliceModeAddon::vertex_shader_addon;
        geometry_source[2] = geometry_shader;
        glShaderSource(geometry,3,geometry_source,NULL);
        glCompileShader(geometry);
        // print compile errors if any
        glGetShaderiv(geometry,GL_COMPILE_STATUS,&success);
        if(!success)
            {
                glGetShaderInfoLog(geometry,512,NULL,info_log);
                std::cout<<"Error::Shader::Geometry::Compilation Failed!"<<std::endl<<info_log<<std::endl;
                return 0;
            }
    }

    // shader program
    program=glCreateProgram();
    glAttachShader(program,vertex);
    glAttachShader(program,fragment);
    if( geometry_shader )
        glAttachShader(program,geometry);
    glLinkProgram(program);
    // print linking errors if any
    glGetProgramiv(program,GL_LINK_STATUS,&success);
    if(!success)
    {
        glGetProgramInfoLog(program,512,NULL,info_log);
        std::cout<<"Error::Shader::Program::Linking Failed!"<<std::endl<<info_log<<std::endl;
        return 0;
    }

    // delete the shaders as they are now linked into our program and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if( geometry_shader )
        glDeleteShader(geometry);

    return program;        
}
