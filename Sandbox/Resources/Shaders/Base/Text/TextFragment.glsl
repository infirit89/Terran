#version 420 core

const float glyph_center = 0.50;
const float outline_center = 0.55;
const vec3 outline_color = vec3(0.0, 0.0, 0.0);

struct VS_OUT
{
	vec4	Color;
	vec2	TexCoords;
};

layout(location = 0) in VS_OUT fsIn;

// TODO change this to an int
layout(location = 2) in flat int f_TexIndex;

layout(location = 0) out vec4 o_Color;

uniform sampler2D u_Samplers[16];


void main() 
{
	int index = int(f_TexIndex);

	vec4 color = texture2D(u_Samplers[index], fsIn.TexCoords);
	float dist = color.r;
	float width = fwidth(dist);
	float alpha = smoothstep(glyph_center - width, glyph_center + width, dist);

	o_Color = vec4(fsIn.Color.rgb, fsIn.Color.a * alpha);

	//float mu = smoothstep(outline_center - width, outline_center + width, dist);
	//vec3 rgb = mix(outline_color, fsIn.Color.rbg, mu);
	//o_Color = vec4(rgb, max(alpha, mu));

	//o_Color = fsIn.Color * vec4(1.0, 1.0, 1.0, texture(u_Samplers[index], fsIn.TexCoords).r);
}
