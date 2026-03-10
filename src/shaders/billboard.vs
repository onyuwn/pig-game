#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Output data ; will be interpolated for each fragment.
out vec2 TexCoords;
out vec3 fragPos;

// Values that stay constant for the whole mesh.
uniform vec3 cameraRightWorldSpace;
uniform vec3 cameraUpWorldSpace;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 billboardPos;
uniform vec2 billboardSize;

void main()
{
	vec3 particleCenter_wordspace = billboardPos;
	
	vec3 vertexPosition_worldspace = 
		particleCenter_wordspace
		+ cameraRightWorldSpace * squareVertices.x * billboardSize.x
		+ cameraUpWorldSpace * squareVertices.y * billboardSize.y;


	// Output position of the vertex
	gl_Position = (projection * view) * vec4(vertexPosition_worldspace, 1.0f);
    fragPos = vertexPosition_worldspace;
    TexCoords = aTexCoords;
}