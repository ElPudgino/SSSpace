#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec2 outUv;
layout (location = 1) flat out int side;

layout(buffer_reference, std430) readonly buffer ProjView{ 
	mat4 projviewMatrix;
};

layout( push_constant ) uniform constants
{	
	ProjView pv;
} PushConstants;

void main() 
{
	//const array of positions for the triangle
	const vec3 positions[36] = vec3[36](
		vec3(-1.f,-1.f, 1.0f),
		vec3(1.f,-1.f, 1.0f),
		vec3(-1.f,1.f, 1.0f),
        vec3(1.f,-1.f, 1.0f),
		vec3(-1.f,1.f, 1.0f),
		vec3(1.f,1.f, 1.0f),

		vec3(-1.f,-1.f, -1.0f),
		vec3(1.f,-1.f, -1.0f),
		vec3(-1.f,1.f, -1.0f),
        vec3(1.f,-1.f, -1.0f),
		vec3(-1.f,1.f, -1.0f),
		vec3(1.f,1.f, -1.0f),

		vec3(-1.f,1.f, -1.0f),
		vec3(1.f,1.f, -1.0f),
		vec3(-1.f,1.f, 1.0f),
        vec3(1.f,1.f, -1.0f),
		vec3(-1.f,1.f, 1.0f),
		vec3(1.f,1.f, 1.0f),

		vec3(-1.f,-1.f, -1.0f),
		vec3(1.f,-1.f, -1.0f),
		vec3(-1.f,-1.f, 1.0f),
        vec3(1.f,-1.f, -1.0f),
		vec3(-1.f,-1.f, 1.0f),
		vec3(1.f,-1.f, 1.0f),

		vec3(1.f,-1.f, -1.0f),
		vec3(1.f,-1.f, 1.0f),
		vec3(1.f,1.f, -1.0f),
        vec3(1.f,-1.f, 1.0f),
		vec3(1.f,1.f, -1.0f),
		vec3(1.f,1.f, 1.0f),

		vec3(-1.f,-1.f, -1.0f),
		vec3(-1.f,-1.f, 1.0f),
		vec3(-1.f,1.f, -1.0f),
        vec3(-1.f,-1.f, 1.0f),
		vec3(-1.f,1.f, -1.0f),
		vec3(-1.f,1.f, 1.0f)
	);

	//const array of uvs for the triangle
	const vec2 uvs[36] = vec2[36](
		vec2(1.0f, 0.0f), 
		vec2(0.0f, 0.0f), 
		vec2(1.f, 1.0f),
        vec2(0.0f, 0.0f), 
		vec2(1.0f, 1.0f), 
		vec2(0.f, 1.0f),

		vec2(0.0f, 0.0f), 
		vec2(1.0f, 0.0f), 
		vec2(0.f, 1.0f),
        vec2(1.0f, 0.0f), 
		vec2(0.0f, 1.0f), 
		vec2(1.f, 1.0f),

		vec2(0.0f, 1.0f), 
		vec2(0.0f, 0.0f), 
		vec2(1.f, 1.0f),
        vec2(0.0f, 0.0f), 
		vec2(1.0f, 1.0f), 
		vec2(1.f, 0.0f),

		vec2(0.0f, 0.0f), 
		vec2(0.0f, 1.0f), 
		vec2(1.f, 0.0f),
        vec2(0.0f, 1.0f), 
		vec2(1.0f, 0.0f), 
		vec2(1.f, 1.0f),

		vec2(0.0f, 0.0f), 
		vec2(1.0f, 0.0f), 
		vec2(0.f, 1.0f),
        vec2(1.0f, 0.0f), 
		vec2(0.0f, 1.0f), 
		vec2(1.f, 1.0f),
		
		vec2(1.0f, 0.0f), 
		vec2(0.0f, 0.0f), 
		vec2(1.f, 1.0f),
        vec2(0.0f, 0.0f), 
		vec2(1.0f, 1.0f), 
		vec2(0.f, 1.0f)
	);

	//output the position of each vertex
	vec4 pos = vec4(positions[gl_VertexIndex], 1.0f);
	gl_Position = PushConstants.pv.projviewMatrix * pos;
	outUv = uvs[gl_VertexIndex];
	side = (gl_VertexIndex - (gl_VertexIndex % 6)) / 6;
}