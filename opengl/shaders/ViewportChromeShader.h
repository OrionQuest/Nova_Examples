namespace NovaBuiltinShaders{

    struct ViewportChromeShader {
        static constexpr char const * name = "ViewportChromeShader";
        static constexpr char const * fragment_shader = R"lang::GLSL(
in vec2 TexCoords;
out vec4 color;

uniform int panes;
uniform float thickness;
uniform float width;
uniform float height;

void main()
{    
     // We divide the thickness percent by two, in order to account for extending it on either side of the line
     vec2 thickness_percent = vec2( thickness / float(width), thickness/float(height) ) / 2.0;

     if( panes == 1)
        color = vec4(1.0, 0.0, 0.0, 0.0);
     else if( panes == 2 ){
        float mask = 0.0;
        if( TexCoords.x > (.5 - thickness_percent.x) && TexCoords.x < (.5 + thickness_percent.x) )
            mask = 1.0;
        color = vec4(1.0, 1.0, 1.0, mask);
     } else if( panes == 4 ) {
        color = vec4(1.0, 0.0, 0.0, 1.0);
     }     

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
