#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    // render scene from light's point of view to build depth map
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
