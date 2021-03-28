#version 330 core

layout(location = 0) in vec3 a_Pos;

uniform vec4 u_Color;

out vec4 o_Color;

void main() 
{
	gl_Position = vec4(a_Pos, 1.0); 
	o_Color = u_Color;
}
