//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
#include "Shader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

using namespace Nova;
//#####################################################################
// Constructor
//#####################################################################
Shader::Shader(GLuint programId) : program(programId) {
    glUseProgram( programId );
}

Shader::~Shader() {
    glUseProgram( 0 );
}

GLuint 
Shader::GetProgramId() const {
    return program;
}

void Shader::SetFloat(const GLchar *name, GLfloat value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform1f(glGetUniformLocation(this->program, name), value);
}
void Shader::SetInteger(const GLchar *name, GLint value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(this->program, name), value);
}

void  Shader::SetUInteger  (const GLchar *name, GLuint value, GLboolean useShader){
    if (useShader)
        this->Use();
    glUniform1ui(glGetUniformLocation(this->program, name), value);
}

void Shader::SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->program, name), x, y);
}
void Shader::SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->program, name), value.x, value.y);
}
void Shader::SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->program, name), x, y, z);
}
void Shader::SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->program, name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->program, name), x, y, z, w);
}
void Shader::SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->program, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(this->program, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

//#####################################################################
// Use
//#####################################################################
void Shader::
Use() const 
{
    glUseProgram(this->program);
}
//#####################################################################
