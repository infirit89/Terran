#version 420 core

struct VS_OUT
{
	vec4	Color;
	vec2	LocalPos;
	float	Thickness;
};

layout(location = 0) in VS_OUT fsIn;

layout(location = 0) out vec4 color;

void main() 
{
	const float r = 1.0;
	//const float rInner = 1.0;

	float dist = sqrt(dot(fsIn.LocalPos, fsIn.LocalPos));
	if(dist > r || dist < 1 - fsIn.Thickness)
		discard;

	float alpha = 1.0 - smoothstep(1.0 - 0.01, 1.0, dist);
	alpha *= smoothstep(1.0 - fsIn.Thickness - 0.01, 1.0 - fsIn.Thickness, dist);
	color = fsIn.Color;
	color.a = fsIn.Color.a * alpha;

	//color = fsIn.Color;
}
