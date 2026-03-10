#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;
uniform float u_time;

void main()
{
    vec4 position = projection * vec4(vertex.xy, 0.0, 1.0);
    position.y = position.y + (.01 * cos(50 * (u_time + (position.x * 10))));
    gl_Position = position;
    TexCoords = vertex.zw;
} 