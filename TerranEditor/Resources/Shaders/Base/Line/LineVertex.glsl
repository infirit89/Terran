#version 420 core


/*layout(location = 0) in vec3 a_Position;
layout(location = 1) in float a_Thickness;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_Normal;

layout(std140, binding = 0) uniform Camera 
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;

} camera;

struct VS_OUT 
{
	vec4 Color;
};

layout(location = 0) out VS_OUT vsOut;

void main() 
{
	vec4 delta = vec4(a_Normal * a_Thickness * 0.5, 0.0, 0.0);
	vec4 pos = vec4(a_Position, 1.0);

	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * (pos + delta);

	vsOut.Color = a_Color;
}*/

layout(location = 0) in vec3	a_Position;
layout(location = 1) in vec3	a_StartPoint;
layout(location = 2) in vec3	a_EndPoint;
layout(location = 3) in vec4	a_Color;
layout(location = 4) in float	a_Thickness;
layout(location = 5) in int		a_EntityID;

layout(std140, binding = 0) uniform Camera 
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;

} camera;


struct VS_OUT 
{
	vec4	Color;
	int		EntityID;
};

layout(location = 0) out VS_OUT vsOut;

void main(void) 
{
	vec2 xBasis = a_EndPoint.xy - a_StartPoint.xy;
	vec2 yBasis = normalize(vec2(-xBasis.y, xBasis.x));
	//vec2 point = a_StartPoint.xy + xBasis * a_Position.x + yBasis * (a_Thickness * 0.1) * a_Position.y;
	vec2 point = a_StartPoint.xy + xBasis * a_Position.x + yBasis * a_Thickness * a_Position.y;
	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * mat4(1.0) * vec4(point, 0.0, 1.0);
	vsOut.Color = a_Color;
	vsOut.EntityID = a_EntityID;

}
