#version 420 core


struct VS_OUT
{
	vec4	Color;
	vec2	TexCoords;
};

layout(location = 0) in VS_OUT fsIn;

layout(location = 2) in flat int f_TexIndex;
layout(location = 3) in flat int f_EntityID;

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

uniform sampler2D u_Samplers[16];


void main() 
{
	int index = f_TexIndex;

	color = fsIn.Color;

	if(index > 0)
		color = fsIn.Color * texture(u_Samplers[index], fsIn.TexCoords);

	entityID = f_EntityID;
}
