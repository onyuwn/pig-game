#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;  
uniform vec3 lightColor;
uniform vec3 playerPos;

void main()
{   
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;
    vec4 objectColor = texture(texture_diffuse1, TexCoords);

    if(objectColor.a < 0.1) {
        discard;
    }

    // vec3 norm = normalize(normal);
    // vec3 lightDir = normalize(lightPos - fragPos); 
    // float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse = diff * lightColor;
    // vec3 result = (ambient + diffuse) * objectColor.xyz;

    FragColor = objectColor;
}