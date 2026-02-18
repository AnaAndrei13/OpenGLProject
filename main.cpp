#include "shader.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cube.h"

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
    cout << cubeVAO << endl;


    glEnable(GL_DEPTH_TEST);

    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");
    if (shader.ID == 0) {
        cerr << "Shader initialization failed. Check previous error messages and shader file paths.\n";
        return -1;
    }


    while (!glfwWindowShouldClose(window))
    {
        // Clean screen
        glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // Model matrix: positions, rotates, and scales the cube
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(5.0f)); // 5x5x5
        model = glm::rotate(model,
            (float)glfwGetTime(),
            glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 view =
            glm::translate(glm::mat4(1.0f),
            glm::vec3(0.0f, 0.0f, -20.0f));
        glm::mat4 projection =
            glm::perspective(glm::radians(45.0f),
            800.0f / 600.0f,
            0.1f,
            1000.0f);

        // Send matrices to shader
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
         
        // Bind the cube VAO and draw the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up and close the window
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
