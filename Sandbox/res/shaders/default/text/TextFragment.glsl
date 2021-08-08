#version 420 core

in vec4 o_Color;
in vec2 o_TexCoords;
in float o_TexIndex;
in float o_TilingIndex; 

uniform sampler2D u_Samplers[16];

layout(location = 0) out vec4 color;

void main() 
{
	int index = int(o_TexIndex);
	color = o_Color * vec4(1.0, 1.0, 1.0, texture(u_Samplers[index], o_TexCoords).r);
}
