#version 450

layout (set = 0, binding = 0) uniform sampler2D texSampler;

//shader input
layout (location = 0) in vec2 uv;
layout (location = 1) in vec3 normal;

//output write
layout (location = 0) out vec4 outFragColor;

void main() 
{
    vec4 clr = texture(texSampler, uv);
    const vec3 ldir = normalize(vec3(1,1,1));
    outFragColor = clr * (dot(normalize(normal), ldir) * 0.4 + 0.6);
}
