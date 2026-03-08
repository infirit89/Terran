#version 450

layout(location = 0) in vec3 i_Color;
layout(location = 1) in vec2 i_TextureCoordinates;

layout(location = 0) out vec4 o_Color;

layout(binding = 1) uniform sampler2D u_Sampler;


void main() 
{
    o_Color = texture(u_Sampler, i_TextureCoordinates) * vec4(i_Color, 1.0);
}
