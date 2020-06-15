//fragment shader of parallax map
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap; //Texture map
uniform sampler2D normalMap; //Normal map
uniform sampler2D depthMap; //Depth map

uniform vec3 lightPos;
uniform vec3 viewPos;

//Used to calculate texture offset (accepts two parameters, texture coordinates and view vector)
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir){ 
    float depth =  texture(depthMap, texCoords).r;
    vec2 p = viewDir.xy * depth / viewDir.z * 0.05; //H£¨A£©/ Pz (viewDir.xy is the texture coordinates at the view vector)
                                                    //0.05 is a correction value because it is an approximate calculation
    return texCoords - p;    //Returns the texture coordinates after offset
} 

void main()
{           
    //Sample parallax
    vec3 viewDir   = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);//Get offset texture coordinates

    //Sample the normal map
    vec3 normal = texture(normalMap, texCoords).rgb;
   //Convert normal vector to [-1,1] range
    normal = normalize(normal * 2.0 - 1.0);  //This normal vector is in tangent space
   
    //Sample Diffuse
    vec3 color = texture(diffuseMap, texCoords).rgb;
    //Ambient light
    vec3 ambient = 0.1 * color;
    //Diffuse reflection
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    //Specular highlight
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}