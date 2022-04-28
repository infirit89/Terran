#version 420 core

//const float glyph_center = 0.50;
//const float outline_center = 0.55;
//const vec3 outline_color = vec3(0.0, 0.0, 0.0);

struct VS_OUT
{
	vec4	TextColor;
	vec2	TexCoords;
};

layout(location = 0) in VS_OUT fsIn;

// TODO change this to an int
layout(location = 2) in flat int i_TexIndex;

layout(location = 0) out vec4 o_Color;

uniform sampler2D u_Samplers[16];

float median(float r, float g, float b) 
{
	return max(min(r, g), min(max(r, g), b));
}

const vec4 bgColor = vec4(0.0, 0.0, 0.0, 0.0);

float screenPxRange() 
{
	vec2 unitRange = vec2(2.0) / vec2(textureSize(u_Samplers[i_TexIndex], 0));
	vec2 screenTextSize = vec2(1.0) / fwidth(fsIn.TexCoords);
	return max(0.5 * dot(unitRange, screenTextSize), 1.0);
}

void main() 
{
	vec3 msd = texture(u_Samplers[i_TexIndex], fsIn.TexCoords).rgb; 
	float sd = median(msd.r, msd.g, msd.b);
	float screenPxDistance = screenPxRange() * (sd - 0.5);
	float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	//o_Color = vec4(1.0, 1.0, 1.0, opacity);
	o_Color = mix(bgColor, fsIn.TextColor, opacity);
	//int index = int(f_TexIndex);

	//vec4 color = texture2D(u_Samplers[index], fsIn.TexCoords);
	//float dist = color.r;
	//float width = fwidth(dist);
	//float alpha = smoothstep(glyph_center - width, glyph_center + width, dist);

	//o_Color = vec4(fsIn.Color.rgb, fsIn.Color.a * alpha);

	//float mu = smoothstep(outline_center - width, outline_center + width, dist);
	//vec3 rgb = mix(outline_color, fsIn.Color.rbg, mu);
	//o_Color = vec4(rgb, max(alpha, mu));

	//o_Color = fsIn.Color * vec4(1.0, 1.0, 1.0, texture(u_Samplers[index], fsIn.TexCoords).r);
}
