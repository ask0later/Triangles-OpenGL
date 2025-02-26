#version 330 core
layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertColor;
layout (location = 2) in vec3 vertNormal;

out vec3 fragColor;
out vec3 fragNormal;
out vec3 fragPos;

uniform mat4 view;
uniform mat4 projection;

void main() 
{
    fragColor = vertColor;
    fragNormal = vertNormal;
    fragPos = vertPos;
    gl_Position = projection * view * vec4(vertPos, 1.0);
}