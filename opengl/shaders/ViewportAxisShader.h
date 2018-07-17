namespace NovaBuiltinShaders{

    struct ViewportAxisShader {
        static constexpr char const * name = "ViewportAxisShader";
        static constexpr char const * fragment_shader = R"lang::GLSL(
out vec4 color;
in vec4 vertex_color;

void main()
{
    color = vertex_color;   
}  
)lang::GLSL";

        static constexpr char const * vertex_shader = R"lang::GLSL(
layout (location = 0) in vec4 position;

void main()
{
    gl_Position = position;
}
)lang::GLSL";
        
        static constexpr char const * geometry_shader = R"lang::GLSL(
layout (points) in;
layout (line_strip, max_vertices = 6) out;

out vec4 vertex_color;

void main() {    

    mat4 final_mat =  projection * view * model;

    gl_Position = final_mat * gl_in[0].gl_Position; 
    vertex_color = vec4( 1.0, 1.0, 1.0, 1.0 );
    EmitVertex();
    gl_Position = final_mat * (gl_in[0].gl_Position + vec4(1.0, 0, 0, 0 )); 
    vertex_color = vec4( 1.0, 0.0, 0.0, 1.0 );
    EmitVertex();
    EndPrimitive();

    gl_Position = final_mat * gl_in[0].gl_Position; 
    vertex_color = vec4( 1.0, 1.0, 1.0, 1.0 );
    EmitVertex();
    gl_Position = final_mat * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0, 0 )); 
    vertex_color = vec4( 0.0, 1.0, 0.0, 1.0 );
    EmitVertex();
    EndPrimitive();

    gl_Position = final_mat * gl_in[0].gl_Position; 
    vertex_color = vec4( 1.0, 1.0, 1.0, 1.0 );
    EmitVertex();
    gl_Position = final_mat * (gl_in[0].gl_Position + vec4(0.0, 0, 1.0, 0 )); 
    vertex_color = vec4( 0.0, 0.0, 1.0, 1.0 );
    EmitVertex();
    EndPrimitive();
}  


)lang::GLSL";
    };
};
