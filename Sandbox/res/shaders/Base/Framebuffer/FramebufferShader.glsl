#type vertex
#version 420 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexCoords;

layout(location = 0) out vec2 o_TexCoords;

void main() 
{
	gl_Position = vec4(a_Pos, 1.0); 

	o_TexCoords = a_TexCoords;
}

#type fragment
#version 420 core

layout(location = 0) in vec2 o_TexCoords;

layout(location = 0) out vec4 color;

uniform sampler2D u_Sampler;

void main() 
{		
	color = texture(u_Sampler, o_TexCoords);
}
