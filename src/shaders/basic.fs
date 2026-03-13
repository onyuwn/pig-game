#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;  
uniform vec3 lightColor;
uniform vec3 playerPos;
uniform float hitTime;
uniform float sceneTime;
uniform float ambientStrength;
uniform float opacity;

vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

void main()
{   
    vec3 ambient = ambientStrength * lightColor;
    vec4 objectColor = texture(texture_diffuse1, TexCoords);

    if(objectColor.a < 0.1) {
        discard;
    }

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    if(sceneTime - hitTime > 0.0) {
        diffuse += vec3(1.0, 0.0, 0.0) * vec3(1 / pow(sceneTime - hitTime, 2.0));
    }
    vec3 result = (ambient + diffuse) * objectColor.xyz;

    if(opacity < 0.75) {
        result *= (random2(TexCoords * opacity).x);
    }

    FragColor = vec4(result, opacity);
}