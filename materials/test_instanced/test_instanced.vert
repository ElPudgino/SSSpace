#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 outColor;

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

	//const array of colors for the triangle
	const vec3 colors[4] = vec3[4](
		vec3(1.0f, 0.0f, 0.0f), //red
		vec3(0.0f, 1.0f, 0.0f), //green
		vec3(1.0f, 1.0f, 0.0f),  //blue
		vec3(0.0f, 0.0f, 0.0f)
	);

	//get vertex base position
	vec4 pos = vec4(PushConstants.vertexBuffer.vertices[gl_VertexIndex].pos, 1.0f);
	//apply transform
	//right now matrix already contains view and projection, may instead use uniform buffer to pass view matrix
	pos = PushConstants.transformArray.matrixes[PushConstants.firstIndex + gl_InstanceIndex] * pos;
	gl_Position = pos;
	outColor = colors[gl_VertexIndex%4];
}
