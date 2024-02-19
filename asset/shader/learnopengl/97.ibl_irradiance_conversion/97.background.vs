#version 450 

layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 WorldPos;

void main() {
	WorldPos = aPos;

	mat4 rotView = mat4(mat3(view)); // 只考虑旋转
	vec4 clipPos = projection * rotView * vec4(aPos, 1.0);

	gl_Position = clipPos.xyww;
}