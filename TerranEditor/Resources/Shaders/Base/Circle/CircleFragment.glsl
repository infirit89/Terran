#version 450 core

struct VS_OUT
{
	vec4	Color;
	vec2	LocalPos;
	float	Thickness;
};

layout(location = 0) in VS_OUT i_InputData;
layout(location = 3) in flat int i_EntityID;

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

void main() 
{
//	const float r = 1.0;
//	//const float rInner = 1.0;
//
//	float dist = length(i_InputData.LocalPos);
//	if(dist > r || dist < 1 - i_InputData.Thickness)
//		discard;
//
//	float alpha = 1.0 - smoothstep(1.0 - 0.01, 1.0, dist);
//	alpha *= smoothstep(1.0 - i_InputData.Thickness - 0.01, 1.0 - i_InputData.Thickness, dist);
//	o_Color = i_InputData.Color;
//	o_Color.a = i_InputData.Color.a * alpha;

	float distance = 1.0 - length(i_InputData.LocalPos);
	float circle = smoothstep(0.0, 0.001, distance);
	circle *= smoothstep(i_InputData.Thickness + 0.001, i_InputData.Thickness, distance);

	if(circle == 0.0)
		discard;

	o_Color = i_InputData.Color;
	o_Color.a *= circle;

	//o_Color = i_InputData.Color;
	o_EntityID = i_EntityID;
}
