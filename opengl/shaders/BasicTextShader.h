namespace NovaBuiltinShaders{

    struct BasicTextShader {
        static constexpr char const * name = "BasicTextShader";
        static constexpr char const * fragment_shader = R"lang::GLSL(
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    color = vec4(textColor, texture(text, TexCoords).r);
}
)lang::GLSL";

        static constexpr char const * vertex_shader = R"lang::GLSL(
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
} 
 
)lang::GLSL";
        static constexpr char const * geometry_shader = nullptr;
    };
};
