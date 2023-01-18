#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 lightColor;
uniform vec3 lightPos; 

uniform vec3 viewPos; 

uniform sampler2D texture_diffuse1;

void main()
{    
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
        
    vec3 objectColor = vec3(texture(texture_diffuse1, TexCoords));
    vec3 result = ambient +( diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}