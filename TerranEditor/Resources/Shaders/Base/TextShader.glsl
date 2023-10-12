// ---- vertex shader ----
#shader_stage vertex
#version 450 core

layout(location = 0) in vec3	a_Position;
layout(location = 1) in int		a_TextureIndex;
layout(location = 2) in vec4	a_TextColor;
layout(location = 3) in vec2	a_TextureCoords;
layout(location = 4) in int		a_EntityID;

layout(std140, binding = 0) uniform Camera 
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;

} camera;

struct VS_OUT
{
	vec4	TextColor;
	vec2	TexCoords;
};

layout(location = 0) out VS_OUT		o_OutputData;
layout(location = 2) out flat int	o_TextureIndex;
layout(location = 3) out flat int	o_EntityID;

void main() 
{
	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * vec4(a_Position, 1.0); 

	o_OutputData.TextColor = a_TextColor;
	o_OutputData.TexCoords = a_TextureCoords;

	o_TextureIndex = a_TextureIndex;
	o_EntityID = a_EntityID;
}


// ---- fragment shader ----
#shader_stage fragment
#version 450 core

//const float glyph_center = 0.50;
//const float outline_center = 0.55;
//const vec3 outline_color = vec3(0.0, 0.0, 0.0);

struct VS_OUT
{
	vec4	TextColor;
	vec2	TexCoords;
};

layout(location = 0) in VS_OUT i_InputData;
layout(location = 2) in flat int i_TextureIndex;
layout(location = 3) in flat int i_EntityID;

layout(location = 0) out vec4	o_Color;
layout(location = 1) out int	o_EntityID;

layout(binding = 0) uniform sampler2D u_Samplers[16];

float median(float r, float g, float b)
{
	return max(min(r, g), min(max(r, g), b));
}

const vec4 bgColor = vec4(0.0);

float screenPxRange()
{
	vec2 unitRange = vec2(2.0) / vec2(textureSize(u_Samplers[i_TextureIndex], 0));
	vec2 screenTextSize = vec2(1.0) / fwidth(i_InputData.TexCoords);
	return max(0.5 * dot(unitRange, screenTextSize), 1.0);
}

void main()
{
	vec3 msd = texture(u_Samplers[i_TextureIndex], i_InputData.TexCoords).rgb; 
	float sd = median(msd.r, msd.g, msd.b);
	float screenPxDistance = screenPxRange() * (sd - 0.5);
	float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

	o_Color = mix(bgColor, i_InputData.TextColor, opacity);

	o_EntityID = i_EntityID;
}
