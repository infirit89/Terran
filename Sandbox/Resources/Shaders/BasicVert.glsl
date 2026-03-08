#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TextureCoordinates;

layout(binding = 0) uniform Camera {
    mat4 Projection;
    mat4 View;
} camera;

layout(location = 0) out vec3 o_Color;
layout(location = 1) out vec2 o_TextureCoordinates;

void main()
{
    vec4 position = vec4(a_Position, 1.0);
    gl_Position = camera.Projection * camera.View * position;
    o_Color = a_Color;
    o_TextureCoordinates = a_TextureCoordinates;
}