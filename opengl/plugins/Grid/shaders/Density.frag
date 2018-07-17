in vec3 voxel_color;
out vec4 color;

void main()
{    
    color = vec4(voxel_color,1.);
}
