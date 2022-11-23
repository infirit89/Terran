#version 420 core

struct VS_OUT 
{
	vec4	Color;
	int		EntityID;
};

layout(location = 0) in VS_OUT fsIn;

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

void main() 
{
	color = fsIn.Color;
	entityID = fsIn.EntityID;
}