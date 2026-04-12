#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec2 uv;

// !!!! vec3 is aligned with 16 bytes !!!!
struct Vertex {
	vec3 pos;
	float pad;
	vec2 normal;
	vec2 uv;
};

// buffer_reference tells that this array is accessed through a device pointer
layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

// one matrix per instance
layout(buffer_reference, std430) readonly buffer TransformArray{
	mat4 matrixes[];
};

layout( push_constant ) uniform constants
{	
	VertexBuffer vertexBuffer;
	TransformArray transformArray;
	uint firstIndex;
} PushConstants;

void main() 
{


	//get vertex base position
	vec4 pos = vec4(PushConstants.vertexBuffer.vertices[gl_VertexIndex].pos, 1.0f);
	//apply transform
	//right now matrix already contains view and projection, may instead use uniform buffer to pass view matrix
	pos = PushConstants.transformArray.matrixes[PushConstants.firstIndex + gl_InstanceIndex] * pos;
	gl_Position = pos;
	uv = PushConstants.vertexBuffer.vertices[gl_VertexIndex].uv;
}
