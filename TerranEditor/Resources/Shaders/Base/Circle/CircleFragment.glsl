#version 420 core

struct VS_OUT
{
	vec4	Color;
	vec2	LocalPos;
	float	Thickness;
};

layout(location = 0) in VS_OUT fsIn;

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

void main() 
{
//	const float r = 1.0;
//	//const float rInner = 1.0;
//
//	float dist = length(fsIn.LocalPos);
//	if(dist > r || dist < 1 - fsIn.Thickness)
//		discard;
//
//	float alpha = 1.0 - smoothstep(1.0 - 0.01, 1.0, dist);
//	alpha *= smoothstep(1.0 - fsIn.Thickness - 0.01, 1.0 - fsIn.Thickness, dist);
//	color = fsIn.Color;
//	color.a = fsIn.Color.a * alpha;

	float distance = 1.0 - length(fsIn.LocalPos);
	float circle = smoothstep(0.0, 0.001, distance);
	circle *= smoothstep(fsIn.Thickness + 0.001, fsIn.Thickness, distance);

	if(circle == 0.0)
		discard;

	color = fsIn.Color;
	color.a *= circle;

	//color = fsIn.Color;
	entityID = 1;
}
