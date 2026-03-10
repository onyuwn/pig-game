#version 330 core

in vec2 texCoords;
out vec4 fragColor;
uniform sampler2D uiTex;
uniform int curCol;
uniform int curRow;
uniform vec2 atlasSize;
uniform vec2 frameSize;

void main() {
    vec2 frameOffset = vec2(curCol, curRow) * frameSize / atlasSize;
    vec2 frameUv = frameOffset + texCoords * frameSize / atlasSize;
    vec4 diffuse = texture(uiTex, frameUv);

    if(diffuse.a < .1) {
        discard;
    }

    fragColor = diffuse;
}