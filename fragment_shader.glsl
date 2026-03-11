#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 VertexColor;

uniform sampler2D texture1; //grass
uniform sampler2D texture2; //sky
uniform sampler2D texture3; //mountain
uniform sampler2D roadTexture;
uniform bool isTerrain;
uniform bool isModel;



void main()
{    vec4 color;
     vec3 a = abs(FragPos);
     vec2 uv;

       if (isModel) {
      FragColor = texture(roadTexture, TexCoord);
      return;
       }
      if (isTerrain) {
       vec4 grass = texture(texture1, TexCoord);
       vec4 rock = vec4(0.5f, 0.4f, 0.3f, 1.0f); 
       float height = clamp(FragPos.y * 10.0f, 0.0f, 1.0f);
       FragColor = mix(grass, rock, height);
      } 
      else
     {
    // determine dominant axis to identify the face reliably
      if (a.y >= a.x && a.y >= a.z) {
         // top or bottom face
         if (FragPos.y < 0.0)
            FragColor = texture(texture1, TexCoord); // bottom -> grass
      else
            FragColor = texture(texture2, TexCoord); // top -> sky

    }
    else if (a.x >= a.y && a.x >= a.z) {
        // left or right faces
        if (FragPos.x > 0.0)
        uv = vec2(1.0 - TexCoord.y, 1.0 - TexCoord.x); // right
        else
        uv = vec2( 1.0 - TexCoord.y, 1.0 - TexCoord.x);        // left

    FragColor = texture(texture3, uv);
     } 

         else {
     // front/back faces 
           if (FragPos.z < 0.0)
             uv = vec2(TexCoord.x, 1.0 - TexCoord.y); // back
          else
            uv = vec2(1.0 - TexCoord.x, 1.0 - TexCoord.y); // front
    FragColor = texture(texture3, uv);
   }
   } 
}
