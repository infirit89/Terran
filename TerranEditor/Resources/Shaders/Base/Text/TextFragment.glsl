#version 450 core

//const float glyph_center = 0.50;
//const float outline_center = 0.55;
//const vec3 outline_color = vec3(0.0, 0.0, 0.0);

struct VS_OUT
{
	vec4	TextColor;
	vec2	TexCoords;
};

layout(location = 0) in VS_OUT a_FsIn;
layout(location = 2) in flat int a_TexIndex;
layout(location = 3) in flat int a_EntityID;

layout(location = 0) out vec4	o_Color;
layout(location = 1) out int	o_EntityID;

uniform sampler2D u_Samplers[16];

float median(float r, float g, float b) 
{
	return max(min(r, g), min(max(r, g), b));
}

const vec4 bgColor = vec4(0.0, 0.0, 0.0, 0.0);

float screenPxRange() 
{
	vec2 unitRange = vec2(5.0) / vec2(textureSize(u_Samplers[a_TexIndex], 0));
	vec2 screenTextSize = vec2(1.0) / fwidth(a_FsIn.TexCoords);
	return max(0.5 * dot(unitRange, screenTextSize), 1.0);
}

void main() 
{
	vec3 msd = texture(u_Samplers[a_TexIndex], a_FsIn.TexCoords).rgb; 
	float sd = median(msd.r, msd.g, msd.b);
	float screenPxDistance = screenPxRange() * (sd - 0.5);
	float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	o_Color = mix(bgColor, a_FsIn.TextColor, opacity);

	o_EntityID = a_EntityID;
}
