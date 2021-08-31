#version 420 core

layout(location = 0) out vec4 color;

uniform sampler2D u_Samplers[16];

in VS_OUT 
{
	vec4	Color;
	vec2	TexCoords;
	float	TexIndex;
} fsIn;

void main() 
{
	int index = int(fsIn.TexIndex);
	color = fsIn.Color * vec4(1.0, 1.0, 1.0, texture(u_Samplers[index], fsIn.TexCoords).r);
}
