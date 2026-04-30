#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec2 outUv;

struct Star{
	vec2 spos;
	float magn;
	uint type;
};

layout(buffer_reference, std430) readonly buffer StarBuffer{ 
	Star stars[];
};

layout( push_constant ) uniform constants
{	
	mat4 projviewMatrix;
	StarBuffer sbuffer;
} PushConstants;

void main() 
{
	//const array of positions for the triangle
	const vec3 positions[6] = vec3[6](
		vec3(-1.f,-1.f, 1.0f),
		vec3(1.f,-1.f, 1.0f),
		vec3(-1.f,1.f, 1.0f),
        vec3(1.f,-1.f, 1.0f),
		vec3(-1.f,1.f, 1.0f),
		vec3(1.f,1.f, 1.0f)
	);

	//const array of colors for the triangle
	const vec2 uvs[6] = vec2[6](
		vec2(0.0f, 0.0f), 
		vec2(1.0f, 0.0f), 
		vec2(0.f, 1.0f),
        vec2(1.0f, 0.0f), 
		vec2(0.0f, 1.0f), 
		vec2(1.f, 1.0f)
	);

	//output the position of each vertex
	vec4 pos = vec4(positions[gl_VertexIndex], 1.0f);
	Star star = PushConstants.sbuffer.stars[gl_InstanceIndex];

	pos.x = pos.x * star.magn;
	pos.y = pos.y * star.magn;
	float anglex = star.spos.x;
	float angley = star.spos.y;

	float ptemp = 0;

	// Rotate quad up/down 
	ptemp = pos.y;
	pos.y = pos.y * cos(anglex) - pos.z * sin(anglex);
	pos.z = ptemp * sin(anglex) + pos.z * cos(anglex);

	// Rotate quad left/right
	ptemp = pos.x;
	pos.x = pos.x * cos(angley) - pos.z * sin(angley);
	pos.z = ptemp * sin(angley) + pos.z * cos(angley);

	pos = PushConstants.projviewMatrix * pos;
	gl_Position = pos;
	outUv = uvs[gl_VertexIndex];
}