#version 330 core
#define M_PI 3.1415926535897932384626433832795
out vec4 FragColor;
in vec2 TexCoords;

uniform float curTime;

void main() {
    vec4 color = vec4(0.0);
    float c = cos(TexCoords.x * 50 + curTime * 5) * sin(TexCoords.y * 50 + curTime * 5);

    //color = vec4(0, .63, 0, 1.0) * vec4(0.0, 1.0 - c, 0.0, 1.0 - c); 
    color = vec4(.5 * cos(curTime) + .5, .5 * cos(curTime + (M_PI/2.0)) + .5, .5 * cos(curTime - + (M_PI/2.0)) + .5, 1.0) * vec4(1.0, 1.0 - c, 1.0, 1.0 - c); 
    if(color.a < 1.0) {
        discard;
    }
    FragColor = color;
}