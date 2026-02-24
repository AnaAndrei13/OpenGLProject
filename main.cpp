#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  
#include "shader.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cube.h"
#include "terrain.h"

using namespace std;

int main()
{
    // Initialize GLFW and create a window
    if (!glfwInit())
    {
        cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Scena", NULL, NULL);
    if (!window)
    {
        cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    glViewport(0, 0, 800, 600);

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

    while (!glfwWindowShouldClose(window))
    {
        // Clean screen
        glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // diagonal orbiting camera so you can see top, sides and occasionally the bottom
        float radius = 2.0f;
        float azimuth = glfwGetTime() * 0.6f;                       // rotates around Y
        float elevation = glm::radians(30.0f) * sin(glfwGetTime() * 0.5f); // oscillates up/down

        float x = radius * cos(elevation) * cos(azimuth);
        float y = radius * sin(elevation);
        float z = radius * cos(elevation) * sin(azimuth);

        // Model matrix: positions, rotates, and scales the cube
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(5.0f)); // 5x5x5

        glm::mat4 view = glm::lookAt(
            glm::vec3(x,y,z),
            glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(0.0f, 1.0f, 0.0f)   
        );

        glm::mat4 projection =
            glm::perspective(glm::radians(90.0f),
            800.0f / 600.0f,
            0.1f,
            1000.0f);


        // Send matrices to shader
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);


        float cubeScale = 5.0f; 
        glm::mat4 terrainModel = glm::mat4(1.0f);
        float bottomY = -cubeScale + 0.01f;
        terrainModel = glm::translate(terrainModel, glm::vec3(0.0f, bottomY, 0.0f)); 
        terrainModel = glm::scale(terrainModel, glm::vec3(cubeScale * 0.98f, 1.0f , cubeScale * 0.98f));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, skyTexture);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mountainTexture);
       
         
        // Bind the cube VAO and draw the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw the terrain
        shader.setBool("isTerrain", true);
        shader.setMat4("model", terrainModel);
        
        glBindVertexArray(terrainVAO);
        glDrawArrays(GL_TRIANGLES, 0, terrainVertexCount);

        shader.setBool("isTerrain", false);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up and close the window
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
