namespace NovaBuiltinShaders{

    struct BasicColored {
        static constexpr char const * name = "BasicColored";
        static constexpr char const * fragment_shader = R"lang::GLSL(
out vec4 color;
uniform vec3 basecolor = vec3( .8, .8, .8);

void main(){
   color = vec4(basecolor, 1.0f);
}

)lang::GLSL";

        static constexpr char const * vertex_shader = R"lang::GLSL(
layout (location=0) in vec3 position;
uniform int  enable_slice = 0;

uniform int autoscale_points = 0;
uniform float point_size = 1.0f;
uniform vec3 camera_pos = vec3(0,0,0);

void main()
{   
    gl_Position=projection*view*model*vec4(position, 1.0f);
    if( autoscale_points != 0 ){
         float dist = distance( position, camera_pos);
         gl_PointSize = (point_size) / dist;
    }
    else
        gl_PointSize = point_size;
    if( enable_slice == 1 )
        ComputeSlice( vec4(position,1.0f) );
    else
        NoSlice();
}
)lang::GLSL";

        static constexpr char const * geometry_shader = NULL;
    };
};
