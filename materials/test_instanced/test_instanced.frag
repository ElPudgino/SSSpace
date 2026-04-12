#version 450

layout (set = 0, binding = 0) uniform sampler2D texSampler;

//shader input
layout (location = 0) in vec2 uv;

//output write
layout (location = 0) out vec4 outFragColor;

void main() 
{
    outFragColor = texture(texSampler, uv);
}
