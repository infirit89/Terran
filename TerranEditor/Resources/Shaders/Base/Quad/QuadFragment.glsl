#version 450 core

struct VS_OUT
{
	vec4	Color;
	vec2	TexCoords;
};

layout(location = 0) in VS_OUT i_InputData;
layout(location = 2) in flat int i_TextureIndex;
layout(location = 3) in flat int i_EntityID;

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

uniform sampler2D u_Samplers[16];

void main() 
{
	int index = i_TextureIndex;

	o_Color = i_InputData.Color;

	if(index > 0)
		o_Color = i_InputData.Color * texture(u_Samplers[index], i_InputData.TexCoords);

	o_EntityID = i_EntityID;
}
