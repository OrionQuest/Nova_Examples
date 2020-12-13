//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
// Class Mesh
//##################################################################### 
#ifndef __ASSIMP_RENDERABLE_MESH_H__
#define __ASSIMP_RENDERABLE_MESH_H__

#include "../../Shader.h"
#include <assimp/types.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Nova{
class Vertex
{
  public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

class Texture
{
  public:
  enum TEXTURE_CHANNEL {
    DIFFUSE=1,
    SPECULAR=2,
    AMBIENT=4,
    EMISSIVE=8,
    NORMALS=16,
    SHININESS=32,
    OPACITY=64,
    LIGHTMAP=128 };
    GLuint id;
    TEXTURE_CHANNEL type;
    aiString path;
    std::string string_type(){
      switch(type){
      case DIFFUSE:
	return std::string("texture_diffuse");
      case SPECULAR:
	return std::string("texture_specular");
      case AMBIENT:
	return std::string("texture_ambient");
      case EMISSIVE:
	return std::string("texture_emissive");
      case NORMALS:
	return std::string("texture_normals");
      case SHININESS:
	return std::string("texture_shininess");
      case OPACITY:
	return std::string("texture_opacity");
      case LIGHTMAP:
	return std::string("texture_lightmap");
      };
      return std::string("unrecognized_texture");
    }
};

class AssimpRenderable_Mesh
{
    GLuint VAO,VBO,EBO;                 // render data
  public:
    std::vector<Vertex> vertices;       // mesh data
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    AssimpRenderable_Mesh(const std::vector<Vertex>& vertices_input,const std::vector<GLuint>& indices_input,const std::vector<Texture>& textures_input);
    ~AssimpRenderable_Mesh() {}

//##################################################################### 
    void Setup_Mesh();
    void Draw( Shader& shader);
//##################################################################### 
};
}
#endif
