namespace NovaBuiltinShaders{

    struct BasicWireframeShader {
        static constexpr char const * name = "BasicWireframeShader";
        static constexpr char const * fragment_shader = R"lang::GLSL(
in vec3 FragPosition;
in vec2 TexCoord;
in vec3 Normal;

out vec4 color;

void main(){
   color = vec4( .8, .8, .8, 1.0f);
}
)lang::GLSL";

        static constexpr char const * vertex_shader = R"lang::GLSL(
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 tex_coord;

out VS_OUT {
    vec3 Normal;
    vec2 TexCoord;
    vec3 FragPosition;
} vs_out;

void main()
{
    mat3 textureFlip;
    textureFlip[0] = vec3( 1,  0, 0);
    textureFlip[1] = vec3( 0, -1, 0);
    textureFlip[2] = vec3( 0,  0, 0);

    
    gl_Position=projection*view*model*vec4(position + normal*0.003f,1.0f);
    vs_out.TexCoord=(textureFlip*vec3(tex_coord,0)).rg;
    vs_out.Normal=normal;
    vs_out.FragPosition=vec3(model*vec4(position,1.0f));
    ComputeSlice(vec4(position,1.0f));
}
)lang::GLSL";
        static constexpr char const * geometry_shader = R"lang::GLSL(
layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

in VS_OUT {
    vec3 Normal;
    vec2 TexCoord;
    vec3 FragPosition;
} gs_in[];

out vec3 FragPosition;
out vec2 TexCoord;
out vec3 Normal;

void main()
{
    gl_Position = gl_in[0].gl_Position;
    FragPosition = gs_in[0].FragPosition;
    TexCoord = gs_in[0].TexCoord;
    Normal = gs_in[0].Normal;

    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    FragPosition = gs_in[1].FragPosition;
    TexCoord = gs_in[1].TexCoord;
    Normal = gs_in[1].Normal;

    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    FragPosition = gs_in[2].FragPosition;
    TexCoord = gs_in[2].TexCoord;
    Normal = gs_in[2].Normal;

    EmitVertex();
    gl_Position = gl_in[0].gl_Position;
    FragPosition = gs_in[0].FragPosition;
    TexCoord = gs_in[0].TexCoord;
    Normal = gs_in[0].Normal;

    EmitVertex();
    EndPrimitive();
    
}


)lang::GLSL";
            };
};
