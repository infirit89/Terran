#version 420 core

struct VS_OUT 
{
	vec4 Color;
};

layout(location = 0) in VS_OUT fsIn;

layout(location = 0) out vec4 color;

void main() 
{
	color = fsIn.Color;
}