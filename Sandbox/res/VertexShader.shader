#version 330 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexCoords;

uniform mat4 u_ProjMat;
uniform mat4 u_ViewMat;

out vec2 o_TexCoords;

void main() 
{
	gl_Position = u_ProjMat * u_ViewMat * vec4(a_Pos, 1.0); 
	o_TexCoords = a_TexCoords;
}
