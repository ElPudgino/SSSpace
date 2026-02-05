#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 outColor;

struct Vertex {
	vec3 pos;
	vec2 uv;
	vec2 normal;
};
// buffer_reference tells that this array is accessed through a device pointer
layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

layout( push_constant ) uniform constants
{	
	VertexBuffer vertexBuffer;
} PushConstants;

void main() 
{

	//const array of colors for the triangle
	const vec3 colors[3] = vec3[3](
		vec3(1.0f, 0.0f, 0.0f), //red
		vec3(0.0f, 1.0f, 0.0f), //green
		vec3(00.f, 0.0f, 1.0f)  //blue
	);

	//output the position of each vertex
	gl_Position = vec4(PushConstants.vertexBuffer.vertices[gl_VertexIndex].pos, 1.0f);
	outColor = colors[gl_VertexIndex%3];
}
