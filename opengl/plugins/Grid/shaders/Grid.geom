layout (points) in;
layout (line_strip,max_vertices=32) out;

uniform float size_multiplier = 1.0;

in VS_OUT{
    float dx;
} gs_in[];

const vec4 cubeVerts[8] = vec4[8](
                                  vec4(-1, -1, -1,  0),     // LB   0
                                  vec4(-1,  1, -1,  0),     // LT   1
                                  vec4( 1, -1, -1,  0),     // RB   2
                                  vec4( 1,  1, -1,  0),     // RT   3
                                  //back face
                                  vec4(-1, -1,  1,  0),     // LB   4
                                  vec4(-1,  1,  1,  0),     // LT   5
                                  vec4( 1, -1,  1,  0),     // RB   6
                                  vec4( 1,  1,  1,  0)      // RT   7
                                  );

const int cubeIndices[16] = int[16](
                                    0,1,3,2,
                                    4,5,7,6,
                                    0,1,5,4,
                                    2,3,7,6
                                    );

void main()
{
    vec4 transVerts[8];
    mat4 final_mat = projection*view*model;

    for(int i=0;i<8;i++) 
    {
        transVerts[i]=(gl_in[0].gl_Position + gs_in[0].dx*size_multiplier*cubeVerts[i]/2.);
    }

    for(int j=0;j<4;++j) for(int i=4*j;i<4*(j+1);++i)
    {
        if(i==4*j+3)
        {
            gl_Position = final_mat*transVerts[cubeIndices[i]];
            gl_ClipDistance[0] = gl_in[0].gl_ClipDistance[0];
            gl_ClipDistance[1] = gl_in[0].gl_ClipDistance[1];
            EmitVertex();

            gl_Position = final_mat*transVerts[cubeIndices[4*j]];
            gl_ClipDistance[0] = gl_in[0].gl_ClipDistance[0];
            gl_ClipDistance[1] = gl_in[0].gl_ClipDistance[1];
            EmitVertex();
        }
        else
        {
            gl_Position = final_mat*transVerts[cubeIndices[i]];
            gl_ClipDistance[0] = gl_in[0].gl_ClipDistance[0];
            gl_ClipDistance[1] = gl_in[0].gl_ClipDistance[1];
            EmitVertex();

            gl_Position = final_mat*transVerts[cubeIndices[i+1]];
            gl_ClipDistance[0] = gl_in[0].gl_ClipDistance[0];
            gl_ClipDistance[1] = gl_in[0].gl_ClipDistance[1];
            EmitVertex();
        }
        EndPrimitive();
    }
}
