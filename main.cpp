#include "stb_image.h"
#include "shader.h"
#include "cube.h"
#include "terrain.h"
#include "model.h"
#include "city.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


using namespace std;


float camX = 0.0f, camZ = 0.0f;
float camY = -49.0f;  // înălțimea pe care stai în interiorul cubului
float camYaw = 0.0f; // rotație stânga/dreapta

float cityOffsetX = -7.0f;
float cityOffsetZ = -5.0f;

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
        float speed = 0.1f;
        if (key == GLFW_KEY_LEFT)  camYaw -= 3.0f;
        if (key == GLFW_KEY_RIGHT) camYaw += 3.0f;

        float newX = camX, newZ = camZ;
        if (key == GLFW_KEY_UP) {
            newX += speed * sin(glm::radians(camYaw));
            newZ -= speed * cos(glm::radians(camYaw));
        }
        if (key == GLFW_KEY_DOWN) {
            newX -= speed * sin(glm::radians(camYaw));
            newZ += speed * cos(glm::radians(camYaw));
        }

        // verifica coliziune cu toate casele din harta
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

        if (!blocked) { camX = newX; camZ = newZ; }
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
    glCullFace(GL_FRONT);

    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");
    if (shader.ID == 0) {
        cerr << "Shader initialization failed. Check previous error messages and shader file paths.\n";
        return -1;
    }


    unsigned int grassTexture, skyTexture, mountainTexture;

    glGenTextures(1, &grassTexture);
    glGenTextures(1, &skyTexture);
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
     data = stbi_load("Grass008.jpg", &width, &height, &nrChannels, 0);
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

    // --- Sky texture ---
    glBindTexture(GL_TEXTURE_2D, skyTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Load image for the sky texture
    data = stbi_load("view.jpg", &width, &height, &nrChannels,0);
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

    // --- Mountain texture ---
    glBindTexture(GL_TEXTURE_2D, mountainTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Load image for the mountain texture
    data = stbi_load("peisaj.jpg", &width, &height, &nrChannels, 0);
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
    shader.setInt("texture2", 1);
    shader.setInt("texture3", 2);
   

  
     unsigned int roadTexture;
    glGenTextures(1, &roadTexture);
    glBindTexture(GL_TEXTURE_2D, roadTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   
   
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("colormap.png", &width, &height, &nrChannels, 0);
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
    
    

    Model roadSide = loadOBJ("road-side.obj");
    Model roadIntersection = loadOBJ("road-intersection.obj");
    Model roadRoundabout = loadOBJ("road-roundabout.obj");
    Model roundaboutBarrier = loadOBJ("road-roundabout-barrier.obj");
    Model constructionCone = loadOBJ("construction-cone.obj");
	Model roadEnd = loadOBJ("road-end.obj");
	Model roadEndBarrier = loadOBJ("road-end-barrier.obj");
    Model roadStraight = loadOBJ("road-straight.obj");
    Model roadStraightBarrier = loadOBJ("road-straight-barrier.obj");
	Model lightSquare = loadOBJ("light-square.obj");
	Model roadCurve = loadOBJ("road-curve.obj");
	Model roadBend = loadOBJ("road-bend.obj");
	Model roadCurveIntersection = loadOBJ("road-curve-intersection.obj");


	Model roadBendBarrier = loadOBJ("road-bend-barrier.obj");
	Model house = loadOBJ("building-type-b.obj");


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
        roadBendBarrier
    };


    while (!glfwWindowShouldClose(window))
    {
        //Clean screen
        glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Model matrix: positions, rotates, and scales the cube
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(50.0f)); // 5x5x5

       /* // diagonal orbiting camera so you can see top, sides and occasionally the bottom
        float radius = 6.0f;
        float azimuth = glfwGetTime() * 0.6f;                       // rotates around Y
        float elevation = glm::radians(30.0f) * sin(glfwGetTime() * 0.5f); // oscillates up/down

        float x = radius * cos(elevation) * cos(azimuth);
        float y = radius * sin(elevation);
        float z = radius * cos(elevation) * sin(azimuth);

       glm::mat4 view = glm::lookAt(
            glm::vec3(0,3,7),
            glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(0.0f, 1.0f, 0.0f)   
        ); 
       */
        
        glm::mat4 projection =
            glm::perspective(glm::radians(90.0f),
            (float)mode->width / (float)mode->height,
            0.1f,
            1000.0f);

        // direcția în care privești
        float dirX = sin(glm::radians(camYaw));
        float dirZ = -cos(glm::radians(camYaw));

        glm::mat4 view = glm::lookAt(
            glm::vec3(camX, camY, camZ),                          
            glm::vec3(camX + dirX, camY, camZ + dirZ),            
            glm::vec3(0.0f, 1.0f, 0.0f)
        );



        // Send matrices to shader
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);


        float cubeScale = 50.0f; 
        glm::mat4 terrainModel = glm::mat4(1.0f);
        float bottomY = -cubeScale + 0.5f;
        terrainModel = glm::translate(terrainModel, glm::vec3(-3.0f, bottomY, 3.0f)); 
        terrainModel = glm::scale(terrainModel, glm::vec3(cubeScale * 0.8f, 1.0f , cubeScale * 0.5f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, skyTexture);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mountainTexture);

        shader.setBool("isModel", true);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, roadTexture);
        shader.setInt("roadTexture", 4);


        float cityOffsetX = -7.0f;   
        float cityOffsetZ = -5.0f;   

        renderCityGrid(
            shader,
            cityModels,
            bottomY,
            cityOffsetX,
            cityOffsetZ
        );

        shader.setBool("isModel", false);
  
        shader.setMat4("model", model);
        
         
        // Bind the cube VAO and draw the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw the terrain
        shader.setBool("isTerrain", true);
        shader.setMat4("model", terrainModel);
        
        glBindVertexArray(terrainVAO);
        glDrawArrays(GL_TRIANGLES, 0, terrainVertexCount);
        shader.setBool("isTerrain", false);

        shader.setMat4("model", model);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up and close the window
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
