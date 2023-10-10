// ---- vertex shader ----
#shader_stage vertex
#version 450 core

layout(location = 0) in vec3	a_Position;
layout(location = 1) in vec4	a_Color;
layout(location = 2) in vec2	a_TextureCoords;
layout(location = 3) in int		a_TextureIndex;
layout(location = 4) in int		a_EntityID;

layout(std140, binding = 0) uniform Camera 
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;

} camera;

struct VS_OUT
{
	vec4	Color;
	vec2	TexCoords;
};

layout(location = 0) out VS_OUT o_OuputData;
layout(location = 2) out flat int o_TextureIndex;
layout(location = 3) out flat int o_EntityID;

void main() 
{
	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * vec4(a_Position, 1.0); 

	o_OuputData.Color = a_Color;
	o_OuputData.TexCoords = a_TextureCoords;
	o_TextureIndex = a_TextureIndex;
	o_EntityID = a_EntityID;
}


// ---- fragment shader ----
#shader_stage fragment
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

layout(binding = 0) uniform sampler2D u_Samplers[16];

void main() 
{
	int index = i_TextureIndex;

	o_Color = texture(u_Samplers[i_TextureIndex], i_InputData.TexCoords) * i_InputData.Color;

	o_EntityID = i_EntityID;
}
