layout (location = 0) in vec3 position;

void main()
{
    mat4 final_mat = projection*view*model;
    gl_Position = final_mat*vec4(position,1.0);
    gl_PointSize = 4;
    ComputeSlice(vec4(position,1.0f));
}
