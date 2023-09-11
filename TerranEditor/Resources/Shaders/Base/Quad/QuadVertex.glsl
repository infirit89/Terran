#version 450 core

layout(location = 0) in vec3	a_Pos;
layout(location = 1) in vec4	a_Col;
layout(location = 2) in vec2	a_TexCoords;
layout(location = 3) in int		a_TexIndex;
layout(location = 4) in int		a_EntityID;

layout(std140, binding = 0) uniform Camera 
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;

} camera;

struct VS_OUT
{
	vec4	Color;
	vec2	TexCoords;
};

layout(location = 0) out VS_OUT o_OuputData;
layout(location = 2) out flat int o_TextureIndex;
layout(location = 3) out flat int o_EntityID;

void main() 
{
	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * vec4(a_Pos, 1.0); 

	o_OuputData.Color = a_Col;
	o_OuputData.TexCoords = a_TexCoords;
	o_TextureIndex = a_TexIndex;
	o_EntityID = a_EntityID;
}
