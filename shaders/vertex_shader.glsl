#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;
layout(location = 3) in vec3 aNormal; 

out vec2 TexCoord;
out vec3 FragPos;
out vec3 VertexColor;
out vec3 Normal;        
out vec3 FragPosWorld;  

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
   vec4 worldPosition = model * vec4(aPos, 1.0);
   gl_Position = projection * view * worldPosition;
   TexCoord = aTexCoord;
   FragPos = vec3(worldPosition);

   VertexColor = aColor;
   FragPosWorld = vec3(worldPosition);

    // transform normal to world space (handles non-uniform scaling)
    Normal = mat3(transpose(inverse(model))) * aNormal;
}
