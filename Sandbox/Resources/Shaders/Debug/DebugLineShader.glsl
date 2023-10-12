// ---- vertex shader ----
#shader_stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

layout(std140, binding = 0) uniform Camera 
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;

} camera;

struct VS_OUT 
{
    vec4 Color;
};

layout(location = 0) out VS_OUT vsOut;

void main() 
{
    gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * vec4(a_Position, 1.0);
    vsOut.Color = a_Color;
}


// ---- fragment shader ----
#shader_stage fragment
#version 450 core

struct VS_OUT 
{
    vec4 Color;
};

layout(location = 0) in VS_OUT fsIn;

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

void main() 
{
    color = fsIn.Color;
    entityID = -1;
}