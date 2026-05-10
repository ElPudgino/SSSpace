#version 450

layout (set = 0, binding = 0) uniform sampler2D texSampler0;
layout (set = 0, binding = 1) uniform sampler2D texSampler1;
layout (set = 0, binding = 2) uniform sampler2D texSampler2;
layout (set = 0, binding = 3) uniform sampler2D texSampler3;
layout (set = 0, binding = 4) uniform sampler2D texSampler4;
layout (set = 0, binding = 5) uniform sampler2D texSampler5;

//shader input
layout (location = 0) in vec2 inUv;
layout (location = 1) flat in int side;

//output write
layout (location = 0) out vec4 outFragColor;

void main() 
{
    if (side == 0) outFragColor = texture(texSampler0, inUv);
    if (side == 1) outFragColor = texture(texSampler1, inUv);
    if (side == 2) outFragColor = texture(texSampler3, inUv);
    if (side == 3) outFragColor = texture(texSampler2, inUv);
    if (side == 4) outFragColor = texture(texSampler4, inUv);
    if (side == 5) outFragColor = texture(texSampler5, inUv);
    
}
