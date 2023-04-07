#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord0;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;


out vec3 v_worldPos;
out vec3 v_normal;
out vec2 v_texcoord0;
out vec3 v_tangent;
out vec3 v_bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(a_position, 1.0);
	
	v_worldPos = vec3(model * vec4(a_position, 1.0));
	
	mat3 modelInvTrans = mat3(transpose(inverse(model)));
	v_normal    = normalize(modelInvTrans * a_normal);
	v_tangent   = normalize(modelInvTrans * a_tangent);
	v_bitangent = normalize(modelInvTrans * a_bitangent);
	
	v_texcoord0 = a_texcoord0;
}
