#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
   vec4 worldPosition = model * vec4(aPos, 1.0);
   gl_Position = projection * view * worldPosition;
   TexCoord = aTexCoord;
   FragPos = aPos; 
   VertexColor = aColor;
}
