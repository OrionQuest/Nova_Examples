//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
// Class Shader
//##################################################################### 
#ifndef __OPENGL_3D_SHADER_H__
#define __OPENGL_3D_SHADER_H__

#include <GL/glew.h>
#include <string>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Nova{
class Shader
{
private:
    GLuint program;         // the shader program id
    
public:
    
    Shader(GLuint programID);
    ~Shader();
    GLuint GetProgramId() const ;
    // Utility functions
    void    SetFloat    (const GLchar *name, GLfloat value, GLboolean useShader = false);
    void    SetInteger  (const GLchar *name, GLint value, GLboolean useShader = false);
    void    SetUInteger  (const GLchar *name, GLuint value, GLboolean useShader = false);
    void    SetVector2f (const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
    void    SetVector2f (const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
    void    SetVector3f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
    void    SetVector3f (const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
    void    SetVector4f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
    void    SetVector4f (const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
    void    SetMatrix4  (const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);
//##################################################################### 
    void Use() const ;
//##################################################################### 
};
}
#endif
