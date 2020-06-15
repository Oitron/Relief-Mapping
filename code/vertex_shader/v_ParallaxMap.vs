//vertex shader of parallax map
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent; //T vector in tangent space
layout (location = 4) in vec3 aBitangent; //B vector in tangent space

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos; //The converted light source position
    vec3 TangentViewPos; //The converted viewpoint position
    vec3 TangentFragPos; //The Converted model vertex position
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N); //Orthogonalization
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));    //Transpose the matrix of TBN
    vs_out.TangentLightPos = TBN * lightPos; //Calculate the converted light position
    vs_out.TangentViewPos  = TBN * viewPos; //Calculate the converted view position
    vs_out.TangentFragPos  = TBN * vs_out.FragPos; //Calculate the converted vertex position
        
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}