#version 330 core
#define MAX_LAMPS 20
uniform vec3 lampPos[MAX_LAMPS];
uniform int numLamps;

out vec4 FragColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 VertexColor;
in vec3 Normal;       
in vec3 FragPosWorld;

uniform sampler2D texture1; //grass
uniform sampler2D roadTexture;
uniform bool isTerrain, isModel, isHouse;

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D shadowMaps[MAX_LAMPS];
uniform mat4 lampSpaceMatrices[MAX_LAMPS];

float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDirection)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0) return 0.0;

    float bias = max(0.005f * (1.0f - dot(normal, lightDirection)), 0.005f);

    // PCF soft shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

float calculateLampShadow(int i, vec3 normal, vec3 lampDir)
{
    vec4 fragPosLampSpace = lampSpaceMatrices[i] * vec4(FragPosWorld, 1.0);
    vec3 projCoords = fragPosLampSpace.xyz / fragPosLampSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0)
        return 0.0;

    float bias = max(0.02 * (1.0 - dot(normal, lampDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMaps[i], 0);

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMaps[i], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main()
{ 
     vec3 a = abs(FragPos);

    if (isModel) {
     vec3 norm = normalize(Normal);

    // ===== main light =====
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(norm, lightDirection), 0.0);

    vec3 viewDir = normalize(viewPos - FragPosWorld);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 ambient = 0.40f * lightColor;

    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPosWorld, 1.0);
    float shadow = 0.0;

    shadow = calculateShadow(fragPosLightSpace, norm, lightDirection);
    vec3 lighting = ambient + (1.0 - shadow) * (diff * 1.0 + spec * 0.05) * lightColor;

        // ===== pillars =====
        for (int i = 0; i < numLamps; i++)
        {
            vec3 toLamp = lampPos[i] - FragPosWorld;
            float dist = length(toLamp);
            vec3 lampDir = normalize(toLamp);

            vec3 spotDirection = vec3(0.0, -1.0, 0.0);
            float theta = dot(lampDir, normalize(-spotDirection));
            float cutoff = cos(radians(22.5));

            if (theta > cutoff)
            {
                float diffLamp = max(dot(norm, lampDir), 0.0);
                float attenuation = 1.0 / (1.0 + 0.35 * dist + 0.44 * dist * dist);
                vec3 lampColor = vec3(1.0, 0.85, 0.4);
                float intensity = smoothstep(cutoff, cutoff + 0.03, theta);

                float lampShadow = calculateLampShadow(i, norm, lampDir);
            
                lighting += lampColor * diffLamp * attenuation * intensity * (1.0 - lampShadow * 0.8) * 12.0;
            }
      }

    vec4 texColor = texture(roadTexture, TexCoord);
    FragColor = vec4(lighting * texColor.rgb, texColor.a);
    return;
   }


   if (isTerrain) {
      vec4 grass = texture(texture1, TexCoord);
      vec4 rock  = vec4(0.5, 0.4, 0.3, 1.0);
      float height = clamp(FragPos.y * 10.0, 0.0, 1.0);
      FragColor = mix(grass, rock, height);  // no lighting at all
      return;
  } 
  else {
    // determine dominant axis to identify the face reliably
     if (a.y >= a.x && a.y >= a.z) {
       // top or bottom face
       if (FragPos.y < 0.0)
           FragColor = texture(texture1, TexCoord); // bottom -> grass
       else{ 
       // top -> sky
          float t = clamp((FragPos.y + 1.0) * 0.5, 0.0, 1.0);
          vec3 skyTop    = vec3(0.55, 0.60, 0.65);
          vec3 skyBottom = vec3(0.80, 0.82, 0.85);
          FragColor = vec4(mix(skyBottom, skyTop, t), 1.0);
      }
     }
    
     else if (a.x >= a.y && a.x >= a.z) {
        // side faces - gradient based on world Y position
        float t = clamp((FragPos.y + 1.0) * 0.5, 0.0, 1.0);
        vec3 skyTop    = vec3(0.55, 0.60, 0.65);
        vec3 skyBottom = vec3(0.80, 0.82, 0.85);
        FragColor = vec4(mix(skyBottom, skyTop, t), 1.0);
    }
    else {
        // front/back - same gradient
        float t = clamp((FragPos.y + 1.0) * 0.5, 0.0, 1.0);
        vec3 skyTop    = vec3(0.55, 0.60, 0.65);
        vec3 skyBottom = vec3(0.80, 0.82, 0.85);
        FragColor = vec4(mix(skyBottom, skyTop, t), 1.0);
    }
  }
}
