#version 450 core

struct VS_OUT 
{
	vec4 Color;
};

layout(location = 0) in VS_OUT a_FsIn;
layout(location = 1) in flat int a_EntityID;

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

void main() 
{
	color = a_FsIn.Color;
	entityID = a_EntityID;
}