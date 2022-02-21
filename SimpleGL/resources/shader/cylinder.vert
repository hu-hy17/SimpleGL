#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform float radius;
uniform float length;

uniform mat4 view;
uniform mat4 localToGlobal;
uniform mat4 projection;

void main(){
	vec3 bPos = vec3(aPos.x * radius, aPos.y * radius, aPos.z * length);
	gl_Position = projection * view * localToGlobal * vec4(bPos, 1.0f);
	FragPos = vec3(localToGlobal * vec4(bPos, 1.0));
	Normal = mat3(localToGlobal) * aNormal;
}