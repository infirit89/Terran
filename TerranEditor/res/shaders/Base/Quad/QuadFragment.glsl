#version 420 core


struct VS_OUT
{
	vec4	Color;
	vec2	TexCoords;
};

layout(location = 0) in VS_OUT fsIn;

// TODO change this to an int
layout(location = 2) in flat float f_TexIndex;

layout(location = 0) out vec4 color;

uniform sampler2D u_Samplers[16];


void main() 
{
	int index = int(f_TexIndex);

	color = fsIn.Color;

	if(index > 0)
		color = fsIn.Color * texture(u_Samplers[index], fsIn.TexCoords);
}
