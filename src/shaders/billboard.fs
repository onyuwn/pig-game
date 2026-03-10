#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 fragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 playerPos;

void main()
{   
    vec2 newUv = TexCoords;
    float dPlayer = (distance(playerPos.xz, fragPos.xz) - 1.0);
    if(dPlayer > .1) {
        newUv.xy *= dPlayer / 10.0;
    }
    vec4 diffuse = texture(texture_diffuse1, newUv);
    //vec4 diffuse = vec4(1.0, 0.0, 0.0, 1.0);

    if(diffuse.a < 0.1) {
        discard;
    }

    FragColor = diffuse;
}