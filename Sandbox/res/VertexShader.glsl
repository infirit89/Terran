#version 330 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec4 a_Col;

uniform mat4 u_ProjMat;
uniform mat4 u_ViewMat;

out vec4 o_Color;

void main() 
{
	gl_Position = u_ProjMat * u_ViewMat * vec4(a_Pos, 1.0); 
	o_Color = a_Col;
}
