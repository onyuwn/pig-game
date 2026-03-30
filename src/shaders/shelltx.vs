#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out float shellHeight;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float shellCount;
uniform float shellLength;
uniform int shellIndex;
uniform float shellAttenuation;
uniform float time;

void main() {
    TexCoords = aTexCoords;
    normal = mat3(transpose(inverse(model))) * aNormal;
    shellHeight = float(shellIndex) / float(shellCount);
    float h = pow(shellHeight, shellAttenuation);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragPos = vec3(model * vec4(aPos, 1.0)); 
    gl_Position.y += (shellLength * h);
}