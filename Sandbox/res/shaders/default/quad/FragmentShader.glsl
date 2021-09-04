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

	if(index == 0)
		color = fsIn.Color;
	else
		color = fsIn.Color * texture(u_Samplers[index], fsIn.TexCoords);

	if(color.a < 0.1) 
		discard;
}
