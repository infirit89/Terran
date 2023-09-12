#version 450 core

layout(location = 0) in vec3	a_WorldPosition;
layout(location = 1) in float	a_Thickness;
layout(location = 2) in vec4	a_Color;
layout(location = 3) in vec2	a_LocalPosition;
layout(location = 4) in int		a_EntityID;

layout(std140, binding = 0) uniform Camera 
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;

} camera;

struct VS_OUT
{
	vec4	Color;
	vec2	LocalPos;
	float	Thickness;
};

layout(location = 0) out VS_OUT o_OutputData;
layout(location = 3) out flat int o_EntityID;

void main()
{
	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * vec4(a_WorldPosition, 1.0); 

	o_OutputData.Color = a_Color;
	o_OutputData.LocalPos = a_LocalPosition;
	o_OutputData.Thickness = a_Thickness;
	o_EntityID = a_EntityID;
}
