//fragment shader of relief map
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D parallaxMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    const float numLayers = 10;//Divide the entire depth area into 10 layers
    float layerDepth = 1.0 / numLayers;//Depth of each layer
    float currentLayerDepth = 0.0;//Current layer depth
    vec2 p = viewDir.xy * 0.1;//Full range of texture coordinates
    vec2 deltaTexCoords = p / numLayers;//Change value of each layer of texture

    vec2 currentTexCoords = texCoords;//the beginning of texture coordinates
    float currentDepthMapValue = texture(parallaxMap, currentTexCoords).r;//the beginning of depth

    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords; //Our P vector points to our eyes, and the texture value needs to change in the opposite direction
        currentDepthMapValue = texture(parallaxMap, currentTexCoords).r;  
        currentLayerDepth += layerDepth;  
    }

    vec2 dtex = deltaTexCoords / 2; //set the Half of the texture step
    float deltaLayerDepth = layerDepth / 2; //set the Half of the depth

    //Calculate the current texture and layer depth
    currentTexCoords += dtex;
    currentLayerDepth -= deltaLayerDepth;

    const int numSearches = 8; //Perform 8 times binary searches
    for (int i = 0; i < numSearches; ++i) {
        //Each search, texture step and depth step will be halved
        dtex /= 2;
        deltaLayerDepth /= 2;

        //Sample the current texture
        currentDepthMapValue = texture(parallaxMap, currentTexCoords).r; 
        if (currentDepthMapValue > currentLayerDepth) {
            //If the current depth is greater than the layer depth, approach to the left
            currentTexCoords  -= dtex;
            currentLayerDepth += deltaLayerDepth;
        }
        else {
            //If the current depth is less than the layer depth, approach to the right
            currentTexCoords  += dtex;
            currentLayerDepth -= deltaLayerDepth;
        }
    }

    return currentTexCoords;   
} 

void main()
{           
    //sample parallax
    vec3 viewDir   = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);

    //sample normal map
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