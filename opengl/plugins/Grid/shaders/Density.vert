layout (location = 0) in vec3 position;
layout (location = 1) in vec3 in_color;

out vec3 voxel_color;

void main()
{
    mat4 final_mat = projection*view*model;
    gl_Position = final_mat*vec4(position,1.0);
    gl_PointSize = 4;
    voxel_color=in_color;
    ComputeSlice(vec4(position,1.0f));
}
