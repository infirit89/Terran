#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec4 a_Col;
layout(location = 2) in float a_TexIndex;

uniform mat4 u_ProjMat;
uniform mat4 u_ViewMat;

out vec4 o_Color;
out vec2 o_TexCoords;
out float o_TexIndex;

void main() 
{
	gl_Position = u_ProjMat * u_ViewMat * vec4(a_Pos, 1.0); 

	o_Color = a_Col;

	o_TexCoords = vec2((a_Pos.x + 1.0) / 2.0, (a_Pos.y + 1.0) / 2.0);
	o_TexIndex = a_TexIndex;
}
