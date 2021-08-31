#version 420 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec4 a_Col;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in float a_TexIndex;

layout(std140, binding = 0) uniform Camera 
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;

} camera;

out VS_OUT
{
	vec4	Color;
	vec2	TexCoords;
	float	TexIndex;
} vsOut;

void main() 
{
	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * vec4(a_Pos, 1.0); 

	vsOut.Color = a_Col;

	vsOut.TexCoords = a_TexCoords;

	vsOut.TexIndex = a_TexIndex;
}
