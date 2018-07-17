namespace NovaBuiltinShaders{

    struct BasicMeshShader {
        static constexpr char const * name = "BasicMeshShader";
        static constexpr char const * fragment_shader = R"lang::GLSL(
in vec3 FragPosition;
in vec2 TexCoord;
in vec3 Normal;

out vec4 color;

// Lighting Information
struct Light {
   vec3 position;
   vec3 color;
};
#define NR_MAX_LIGHTS 4
uniform Light lights[NR_MAX_LIGHTS];
uniform int activeLights;
uniform vec3 cameraPos = vec3( 1.0, 0.0, 0.0 );

// Texture and Object colors
struct ColorChannels{
   float opacity;
   vec3 diffuse;
   vec3 specular;
   vec3 ambient;
   vec3 emissive;
   float shininess;
   vec3 lightmap;
};

uniform ColorChannels defaultChannels = ColorChannels(1.0, 
                                                      vec3(1.0,1.0,1.0),
                                                      vec3(1.0,1.0,1.0),
                                                      vec3(0.25,0.25,0.25),
                                                      vec3(0.0,0.0,0.0),
                                                      16.0,
                                                      vec3(1.0,1.0,1.0) );
uniform uint texture_flags = 0u;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normals;
uniform sampler2D texture_specular;
uniform sampler2D texture_ambient;
uniform sampler2D texture_emissive;
uniform sampler2D texture_shininess;
uniform sampler2D texture_opacity;
uniform sampler2D texture_lightmap;

// Control Flags
uniform int selected;
uniform int shaded;

vec3 CalcPhongLight( Light light, vec3 normal, vec3 viewPos, ColorChannels channels ){

       // Basic Direction information
       vec3 norm = normalize(normal);
       vec3 lightDir = normalize(light.position - FragPosition);  
       vec3 viewDir = normalize(viewPos - FragPosition);
       vec3 halfwayDir = normalize(lightDir + viewDir);

       // Diffuse shading
       float diff = max(dot(norm, lightDir), 0.0);

       // Specular shading
       //vec3 reflectDir = reflect(-lightDir, normal);
       float spec = pow(max(dot(norm, halfwayDir), 0.0), channels.shininess);

       // Merge all terms*weights
       vec3 ambient  = channels.ambient*channels.diffuse;
       vec3 diffuse  = diff * channels.diffuse;
       vec3 specular = spec * channels.specular;
    
       return (channels.lightmap * (ambient + diffuse + specular) + channels.emissive);
}

vec3 CalcFlatLight( Light light, vec3 normal, vec3 viewPos, ColorChannels channels ){
       vec3 lightDirection = normalize(light.position - viewPos);
       float diff = max(0.0, dot(lightDirection, normal));

       vec3 ambient  = channels.ambient*channels.diffuse;
       vec3 diffuse  = diff * channels.diffuse;

       return (channels.lightmap * (ambient + diffuse) + channels.emissive);
}

void main()
{    
    ColorChannels channels;
    
    channels.opacity = defaultChannels.opacity;
    if( uint(texture_flags & uint(64)) != 0u)
        channels.opacity = texture(texture_opacity,TexCoord).r;

    channels.diffuse = defaultChannels.diffuse;
    if( (texture_flags & uint(1)) != 0u )
        channels.diffuse = texture(texture_diffuse,TexCoord).rgb;

    channels.specular = defaultChannels.specular;
    if( (texture_flags & uint(2)) != 0u )
        channels.specular = texture(texture_specular,TexCoord).rgb;

    channels.ambient = defaultChannels.ambient;
    if( (texture_flags & uint(4)) != 0u )
        channels.ambient = texture(texture_ambient,TexCoord).rgb;

    vec3 normals_term = Normal;
    if( (texture_flags & uint(16)) != 0u )
        normals_term = texture(texture_normals,TexCoord).rgb;

    channels.emissive = defaultChannels.emissive;
    if( (texture_flags & uint(8)) != 0u )
        channels.emissive = texture(texture_emissive,TexCoord).rgb;

    channels.shininess = defaultChannels.shininess;
    if( (texture_flags & uint(32)) != 0u )
        channels.shininess = texture(texture_shininess,TexCoord).r;

    channels.lightmap = defaultChannels.lightmap;
    if( (texture_flags & uint(128)) != 0u )
        channels.lightmap = texture(texture_lightmap,TexCoord).rgb;


    if( !gl_FrontFacing ){
        channels.diffuse = 1.0f - channels.diffuse;
    }

    if( selected != 0 ){
        channels.diffuse = channels.diffuse * vec3(1.2,0.7,0.7);
    }
    
    // Compute Lighting...
    vec3 result = vec3(0.0,0.0,0.0);
    if( shaded == 1){
        for( int l = 0; l < activeLights; l++ )
            result += CalcPhongLight( lights[l], normals_term, cameraPos, channels );
    }
    else if( shaded == 2){
        // This is a screen-space flat shading method that avoids extra vertices
        vec3 X = dFdx(FragPosition);
        vec3 Y = dFdy(FragPosition);
        vec3 normal=normalize(cross(X,Y));
        for( int l = 0; l < activeLights; l++ )
            result += CalcFlatLight( lights[l], normal, FragPosition, channels );        
    }
    else{
       result = channels.diffuse;
    }
    color = vec4(result, channels.opacity);

}
)lang::GLSL";

        static constexpr char const * vertex_shader = R"lang::GLSL(
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 tex_coord;

out vec3 FragPosition;
out vec2 TexCoord;
out vec3 Normal;

void main()
{
    mat3 textureFlip;
    textureFlip[0] = vec3( 1,  0, 0);
    textureFlip[1] = vec3( 0, -1, 0);
    textureFlip[2] = vec3( 0,  0, 0);

    
    gl_Position=projection*view*model*vec4(position,1.0f);
    TexCoord=(textureFlip*vec3(tex_coord,0)).rg;
    Normal=normal;
    FragPosition=vec3(model*vec4(position,1.0f));
    ComputeSlice(vec4(position,1.0f));
}
)lang::GLSL";
        static constexpr char const * geometry_shader = nullptr;
            };
};
