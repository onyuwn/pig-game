#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec2 tex>
layout(location = 1) in vec2 uv;

out vec2 texCoords;

uniform mat4 projection;

void main()
{
    vec2 coord = vertex.xy / vec2(800,600);
    coord.y = (1.0 - coord.y);
    coord.xy = coord.xy * 2.0 - 1.0;
    //gl_Position = vec4(coord.xy, 0.0, 1.0);
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    texCoords = uv;
}