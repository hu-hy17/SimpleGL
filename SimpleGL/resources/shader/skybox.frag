#version 330 core

uniform samplerCube skyBox;

in vec3 FragPos;

out vec4 FragColor;

void main()
{
    FragColor = texture(skyBox, FragPos);
}