#pragma shader_stage(vertex)
#version 450 core

void main() 
{
    gl_Position = vec4(1.0, 1.0, 1.0, 0.0);
}

#pragma shader_stage(fragment)
#version 450 core

layout(location = 0) out vec4 o_Color;

void main() 
{
    o_Color = vec4(1.0, 0.0, 0.0, 1.0);
}