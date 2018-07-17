layout (location = 0) in vec3 position;
layout (location = 1) in float h;

out VS_OUT {
    float dx;
} vs_out;

void main()
{
    gl_Position = vec4(position,1.0);
    vs_out.dx = h;
    ComputeSlice(vec4(position,1.0f));
}
