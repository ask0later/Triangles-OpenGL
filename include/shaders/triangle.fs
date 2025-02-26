#version 330 core

in vec3 fragColor;
in vec3 fragNormal;
in vec3 fragPos;

out vec4 finalColor;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = dot(norm, lightDir);
    vec3 diffuse = diff * lightColor;

    vec3 result;
    if (gl_FrontFacing) {
        result = (ambient + diffuse) * fragColor;
    } else {
        result = (ambient) * fragColor;
    }

    finalColor = vec4(result, 1.0f);
}