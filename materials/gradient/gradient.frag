#version 450

//shader input
layout (location = 0) in vec3 inColor;

//output write
layout (location = 0) out vec4 outFragColor;

//push constants block
layout( push_constant ) uniform constants
{
    float val;
} PushConstants;

void main() 
{
    outFragColor = vec4(inColor * PushConstants.val,1.0f);
}
