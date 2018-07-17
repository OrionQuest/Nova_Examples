layout (location = 0) in vec3 position;
layout (location = 1) in vec3 offset;

void main()
{
    gl_Position = projection*view*model*vec4(position+offset,1.0);
    ComputeSlice(vec4(offset,1.0f));
}
