#include "stb_image.h"
#include "shader.h"
#include "cube.h"
#include "terrain.h"
#include "model.h"
#include "city.h"
#include "car.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
using namespace std;

float camX = 0.0f, camY = -49.0f, camZ = 0.0f;     
float camYaw = 0.0f;    // rotation around Y axis
float camPitch = 0.0f;  // rotation around X axis
float camRoll = 0.0f;   // rotation around Z axis

float cityOffsetX = -7.0f;
float cityOffsetZ = -5.0f;

Car car;    
bool followCar = false;
float camCarYaw = 0.0f; 

int main()
{
    // Initialize GLFW and create a window
    if (!glfwInit())
    {
        cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Scena", NULL, NULL);
    if (!window)
    {
        cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int, int action, int) {
        float speed = 0.2f;

        if (key == GLFW_KEY_C && action == GLFW_PRESS) followCar = !followCar;

        // --- ROTATIONS ---
        if (key == GLFW_KEY_LEFT)  camYaw -= 3.0f;   // yaw left
        if (key == GLFW_KEY_RIGHT) camYaw += 3.0f;   // yaw right
        if (key == GLFW_KEY_R) camPitch += 2.0f;     // pitch up
        if (key == GLFW_KEY_F) camPitch -= 2.0f;     // pitch down
        if (key == GLFW_KEY_Z) camRoll += 2.0f;      // roll left
        if (key == GLFW_KEY_X) camRoll -= 2.0f;      // roll right
        camPitch = glm::clamp(camPitch, -89.0f, 89.0f); // prevent gimbal flip

        // --- MOVEMENT ON ALL 3 AXES ---
        float newX = camX, newY = camY, newZ = camZ;

        // move forward/backward (Z axis)
        if ((key == GLFW_KEY_UP || key == GLFW_KEY_W) && !followCar){
            newX += speed * cos(glm::radians(camPitch)) * sin(glm::radians(camYaw));
            newY += speed * sin(glm::radians(camPitch));
            newZ -= speed * cos(glm::radians(camPitch)) * cos(glm::radians(camYaw));
        }
        if ((key == GLFW_KEY_DOWN || key == GLFW_KEY_S) && !followCar){
            newX -= speed * cos(glm::radians(camPitch)) * sin(glm::radians(camYaw));
            newY -= speed * sin(glm::radians(camPitch));
            newZ += speed * cos(glm::radians(camPitch)) * cos(glm::radians(camYaw));
        }

        // strafe left/right (X axis)
        if (key == GLFW_KEY_A) {
            newX -= speed * cos(glm::radians(camYaw));
            newZ -= speed * sin(glm::radians(camYaw));
        }
        if (key == GLFW_KEY_D) {
            newX += speed * cos(glm::radians(camYaw));
            newZ += speed * sin(glm::radians(camYaw));
        }

        // move up/down (Y axis)
        if (key == GLFW_KEY_Q) newY += speed * 5.0f;  // move up
        if (key == GLFW_KEY_E) newY -= speed * 5.0f;  // move down

        // check collision with houses
        bool blocked = false;
        for (int row = 0; row < MAP_ROWS && !blocked; row++) {
            for (int col = 0; col < MAP_COLS && !blocked; col++) {
                if (cityMap[row][col] == HOUSE_S || cityMap[row][col] == HOUSE_L) {
                    float hx = cityOffsetX + col * 1.0f;
                    float hz = cityOffsetZ + row * 1.0f;
                    if (abs(newX - hx) < 1.0f && abs(newZ - hz) < 1.0f)
                        blocked = true;
                }
            }
        }

        if (!blocked) { camX = newX; camY = newY; camZ = newZ; }
        });


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    glViewport(0, 0, mode->width, mode->height);

    // Setup cube data and buffers
    setupCube();
    setupTerrain();
    cout << cubeVAO << endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Shader shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    if (shader.ID == 0) {
        cerr << "Shader initialization failed. Check previous error messages and shader file paths.\n";
        return -1;
    }
    Shader depthShader("shaders/depth_vertex.glsl", "shaders/depth_fragment.glsl");

    shader.use();
    unsigned int grassTexture,mountainTexture;

    glGenTextures(1, &grassTexture);
    glGenTextures(1, &mountainTexture);

    int width, height, nrChannels;
    unsigned char* data = nullptr;

    // Grass texture 
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Load image for the grass texture
    data = stbi_load("textures/Grass008.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture!" << endl;
    }
    stbi_image_free(data);

    shader.use();
    shader.setInt("texture1", 0);
    
    unsigned int roadTexture;
    glGenTextures(1, &roadTexture);
    glBindTexture(GL_TEXTURE_2D, roadTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("textures/colormap.png", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        cout << "Failed to load!" << endl;
    }
    stbi_image_free(data);
    stbi_set_flip_vertically_on_load(false);


    Model roadSide = loadOBJ("objects/road-side.obj");
    Model roadIntersection = loadOBJ("objects/road-intersection.obj");
    Model roadRoundabout = loadOBJ("objects/road-roundabout.obj");
    Model roundaboutBarrier = loadOBJ("objects/road-roundabout-barrier.obj");
    Model constructionCone = loadOBJ("objects/construction-cone.obj");
    Model roadEnd = loadOBJ("objects/road-end.obj");
    Model roadEndBarrier = loadOBJ("objects/road-end-barrier.obj");
    Model roadStraight = loadOBJ("objects/road-straight.obj");
    Model roadStraightBarrier = loadOBJ("objects/road-straight-barrier.obj");
    Model lightSquare = loadOBJ("objects/light-square.obj");
    Model roadCurve = loadOBJ("objects/road-curve.obj");
    Model roadBend = loadOBJ("objects/road-bend.obj");
    Model roadCurveIntersection = loadOBJ("objects/road-curve-intersection.obj");
    Model roadBendBarrier = loadOBJ("objects/road-bend-barrier.obj");
    Model house = loadOBJ("objects/building-type-b.obj");
    Model constructionLight = loadOBJ("objects/construction-light.obj");
	Model crossIntersection = loadOBJ("objects/road-intersection.obj");
	Model crossIntersectionBarrier = loadOBJ("objects/road-intersection-barrier.obj");
	Model crossRoad = loadOBJ("objects/road-crossroad.obj");
	Model sign = loadOBJ("objects/sign-highway-detailed.obj");

	Model carModel = loadOBJ("objects/sedan.obj");

    CityModels cityModels = {
        roadStraight,
        roadStraightBarrier,
        roadCurve,
        roadIntersection,
        roadCurveIntersection,
        roadRoundabout,
        roundaboutBarrier,
        constructionCone,
        lightSquare,
        house,
        roadBend,
        roadBendBarrier,
        constructionLight,
        crossIntersection,
        crossIntersectionBarrier,
        crossRoad,
        sign
      
    };

    float cubeScale = 50.0f;
    float bottomY = -cubeScale + 0.5f;
    float lampOffsetX = 0.08f;
    float lampOffsetZ = 0.09f;
    initCar(car, cityOffsetX, cityOffsetZ, bottomY);

   // collect the positions of the pillars
    std::vector<glm::vec3> lampPositions;
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            if (cityMap[row][col] == LIGHT_TILE) {
                float lampX = cityOffsetX + col * 1.0f + lampOffsetX;
                float lampZ = cityOffsetZ + row * 1.0f + lampOffsetZ;
                lampPositions.push_back(glm::vec3(
                    lampX,
                    bottomY + 3.5f,  
                    lampZ
                ));
            }
        }
    }


    const unsigned int SHADOW_WIDTH = 4096;
    const unsigned int SHADOW_HEIGHT = 4096;

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0,1.0,1.0,1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, depthMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    int numLamps = lampPositions.size();

    std::vector<unsigned int> depthMapFBOs(numLamps);
    std::vector<unsigned int> depthMaps(numLamps);
    std::vector<glm::mat4> lampSpaceMatrices(numLamps);

    glGenFramebuffers(numLamps, depthMapFBOs.data());
    glGenTextures(numLamps, depthMaps.data());

    for (int i = 0; i < numLamps; i++) {
        glBindTexture(GL_TEXTURE_2D, depthMaps[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, depthMaps[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(50.0f));

        glm::mat4 terrainModel = glm::mat4(1.0f);
        terrainModel = glm::translate(terrainModel, glm::vec3(-3.0f, bottomY, 3.0f));
        terrainModel = glm::scale(terrainModel, glm::vec3(cubeScale * 0.8f, 1.0f, cubeScale * 0.5f));

        // directional light (sun)
        glm::vec3 lightPosition = glm::vec3(
            cityOffsetX + MAP_COLS * 0.5f - 20.0f,
            bottomY + 8.0f,
            cityOffsetZ + MAP_ROWS * 0.5f - 10.0f
        );

        glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.8f);

        glm::vec3 lightTarget = glm::vec3(
            cityOffsetX + MAP_COLS * 0.5f,
            bottomY,
            cityOffsetZ + MAP_ROWS * 0.5f
        );

        glm::vec3 lightDirection = glm::normalize(lightTarget - lightPosition);

        // light space matrix for shadow mapping
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 30.0f);
        glm::mat4 lightView = glm::lookAt(
            lightPosition,
            lightTarget,  
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

      // ==============================================
      //  PASS 1: render from light view (depth only)
      // ==============================================
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        depthShader.use();
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // render cube to depth buffer
        depthShader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render terrain to depth buffer
        depthShader.setMat4("model", terrainModel);
        glBindVertexArray(terrainVAO);
        glDrawArrays(GL_TRIANGLES, 0, terrainVertexCount);

        // render city models to depth buffer
        renderCityGrid(depthShader, cityModels, bottomY, cityOffsetX, cityOffsetZ);
      
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

       // ==============================================
       // PASS 1.b: render from light lamp
       // ==============================================

        for (int i = 0; i < numLamps; i++) {
            glm::vec3 lampPos = lampPositions[i];
            glm::vec3 lampTarget = glm::vec3(lampPos.x, bottomY, lampPos.z);

            glm::mat4 lampProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 20.0f);
            glm::mat4 lampView = glm::lookAt(lampPos, lampTarget, glm::vec3(0.0f, 0.0f, 1.0f));
            lampSpaceMatrices[i] = lampProj * lampView;

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs[i]);
            glClear(GL_DEPTH_BUFFER_BIT);

            depthShader.use();
            depthShader.setMat4("lightSpaceMatrix", lampSpaceMatrices[i]);

            depthShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            depthShader.setMat4("model", terrainModel);
            glBindVertexArray(terrainVAO);
            glDrawArrays(GL_TRIANGLES, 0, terrainVertexCount);

            renderCityGrid(depthShader, cityModels, bottomY, cityOffsetX, cityOffsetZ);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }


        // ==============================================
        // PASS 2: render scene normally with shadows
        // ==============================================
        glViewport(0, 0, mode->width, mode->height);
        glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setInt("numLamps", numLamps);

        for (int i = 0; i < (int)lampPositions.size(); i++) {
            std::string base = "lampPos[" + std::to_string(i) + "]";
            shader.setVec3(base, lampPositions[i]);
        }

        // camera direction from yaw, pitch and roll
        float dirX = cos(glm::radians(camPitch)) * sin(glm::radians(camYaw));
        float dirY = sin(glm::radians(camPitch));
        float dirZ = -cos(glm::radians(camPitch)) * cos(glm::radians(camYaw));

        glm::vec3 up = glm::vec3(sin(glm::radians(camRoll)),
            cos(glm::radians(camRoll)), 0.0f);
   

        if (followCar) {
            updateCar(car, window, cityOffsetX, cityOffsetZ);
            updateCameraFollowCar(car, camX, camY, camZ);
        }

        glm::mat4 view;

        if (followCar) {
            view = glm::lookAt(
                glm::vec3(camX, camY, camZ),
                car.position,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
        }
        else {
            float dirX = cos(glm::radians(camPitch)) * sin(glm::radians(camYaw));
            float dirY = sin(glm::radians(camPitch));
            float dirZ = -cos(glm::radians(camPitch)) * cos(glm::radians(camYaw));

            glm::vec3 up = glm::vec3(
                sin(glm::radians(camRoll)),
                cos(glm::radians(camRoll)),
                0.0f
            );

            view = glm::lookAt(
                glm::vec3(camX, camY, camZ),
                glm::vec3(camX + dirX, camY + dirY, camZ + dirZ),
                up
            );
        }

        glm::mat4 projection = glm::perspective(glm::radians(90.0f),
            (float)mode->width / (float)mode->height, 0.01f, 500.0f);

        // send all uniforms to shader
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("lightDir", -lightDirection);
        shader.setVec3("lightColor", lightColor);
        shader.setVec3("viewPos", glm::vec3(camX, camY, camZ));
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
   
        for (int i = 0; i < numLamps; i++) {
            glActiveTexture(GL_TEXTURE8 + i);
            glBindTexture(GL_TEXTURE_2D, depthMaps[i]);
            shader.setInt("shadowMaps[" + std::to_string(i) + "]", 8 + i);
            shader.setMat4("lampSpaceMatrices[" + std::to_string(i) + "]", lampSpaceMatrices[i]);
        }
        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, roadTexture);
        shader.setInt("roadTexture", 4);

        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        shader.setInt("shadowMap", 6);

        // draw city models
        shader.setBool("isModel", true);
        renderCityGrid(shader, cityModels, bottomY, cityOffsetX, cityOffsetZ);
        shader.setBool("isModel", false);

        shader.setBool("isModel", true);
        renderCar(car, carModel, shader);
        shader.setBool("isModel", false);

        // draw skybox cube
        glCullFace(GL_FRONT);
        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw terrain
        shader.setBool("isTerrain", true);
        shader.setMat4("model", terrainModel);
        glBindVertexArray(terrainVAO);
        glDrawArrays(GL_TRIANGLES, 0, terrainVertexCount);
        shader.setBool("isTerrain", false);
        glCullFace(GL_BACK);
        shader.setMat4("model", model);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}