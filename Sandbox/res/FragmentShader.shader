#version 410 core

in vec2 o_TexCoords;

uniform sampler2D u_Sampler;

out vec4 color;

void main() 
{
	color = texture(u_Sampler, o_TexCoords);
}
