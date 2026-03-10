#version 330 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D uiTex;

void main() {
    vec4 diffuse = texture(uiTex, texCoords);

    if(diffuse.a < .1) {
        discard;
        //fragColor = vec4(1.0,0,0,1.0);
    }

    fragColor = diffuse;
}