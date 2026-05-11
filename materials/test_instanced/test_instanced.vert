#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec2 outUv;
layout (location = 1) out vec3 outNormal;

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

layout(buffer_reference, std430) readonly buffer Uniforms{
	mat4 matrixes[];
};

layout( push_constant ) uniform constants
{	
	VertexBuffer vertexBuffer;
	TransformArray transformArray;
	Uniforms uniforms;
	uint firstIndex;
} PushConstants;

void main() 
{

	Vertex vert = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	//get vertex base position
	vec4 pos = vec4(vert.pos, 1.0f);
	//apply transform
	//right now matrix already contains view and projection, may instead use uniform buffer to pass view matrix
	mat4 world = PushConstants.transformArray.matrixes[PushConstants.firstIndex + gl_InstanceIndex];
	pos = world * pos;
	pos = PushConstants.uniforms.matrixes[0] * PushConstants.uniforms.matrixes[1] * pos; 
	gl_Position = pos;
	outUv = PushConstants.vertexBuffer.vertices[gl_VertexIndex].uv;
	vec3 normal = vec3(vert.pad, vert.normal);
	normal = transpose(inverse(mat3(world))) * normal;
	outNormal = normal;
}
