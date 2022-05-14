#version 420 core

layout(location = 0) in vec3	a_WorldPos;
layout(location = 1) in int		a_TexIndex;
layout(location = 2) in vec4	a_TextColor;
layout(location = 3) in vec2	a_TexCoords;

layout(std140, binding = 0) uniform Camera 
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;

} camera;

struct VS_OUT
{
	vec4	TextColor;
	vec2	TexCoords;
};

layout(location = 0) out VS_OUT vsOut;
layout(location = 2) out flat int i_TexIndex;

void main() 
{
	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * vec4(a_WorldPos, 1.0); 

	vsOut.TextColor = a_TextColor;
	vsOut.TexCoords = a_TexCoords;
	i_TexIndex = a_TexIndex;
}
