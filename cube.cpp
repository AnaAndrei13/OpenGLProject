#include "cube.h"
#include <glad/glad.h>

/* Define a cube with 6 faces, each face is made of 2 triangles,
   and each triangle has 3 vertices,so we have 6*2*3 = 36 vertices in total. */

 const GLfloat cubeVertices[] = {
     // x    y     z          u   v
        // (z = -1)
	  -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,
	   1.0f,  1.0f, -1.0f,    1.0f, 1.0f,
	   1.0f, -1.0f, -1.0f,    1.0f, 0.0f,
	   1.0f,  1.0f, -1.0f,    1.0f, 1.0f,
	  -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,
	  -1.0f,  1.0f, -1.0f,    0.0f, 1.0f,

       // (z = 1)
	  -1.0f, -1.0f, 1.0f,     0.0f, 0.0f,
	   1.0f, -1.0f, 1.0f,     1.0f, 0.0f,
	   1.0f,  1.0f, 1.0f,     1.0f, 1.0f,
	   1.0f,  1.0f, 1.0f,     1.0f, 1.0f,
	  -1.0f,  1.0f, 1.0f,     0.0f, 1.0f,
	  -1.0f, -1.0f, 1.0f,     0.0f, 0.0f,

      // (x = -1)
	  -1.0f,  1.0f,  1.0f,    1.0f, 0.0f,
	  -1.0f,  1.0f, -1.0f,    1.0f, 1.0f,
	  -1.0f, -1.0f, -1.0f,    0.0f, 1.0f,
	  -1.0f, -1.0f, -1.0f,    0.0f, 1.0f,
	  -1.0f, -1.0f,  1.0f,    0.0f, 0.0f,
	  -1.0f,  1.0f,  1.0f,    1.0f, 0.0f,

     // (x = 1)
	   1.0f,  1.0f,  1.0f,    1.0f, 0.0f,
	   1.0f, -1.0f, -1.0f,    0.0f, 1.0f,
	   1.0f,  1.0f, -1.0f,    1.0f, 1.0f,
	   1.0f, -1.0f, -1.0f,    0.0f, 1.0f,
	   1.0f,  1.0f,  1.0f,    1.0f, 0.0f,
	   1.0f, -1.0f,  1.0f,    0.0f, 0.0f,

     // (y = -1)
	 -1.0f, -1.0f, -1.0f,     0.0f, 0.0f,
	  1.0f, -1.0f, -1.0f,     1.0f, 0.0f,
	  1.0f, -1.0f,  1.0f,     1.0f, 1.0f,
	  1.0f, -1.0f,  1.0f,     1.0f, 1.0f,
	 -1.0f, -1.0f,  1.0f,     0.0f, 1.0f,
	 -1.0f, -1.0f, -1.0f,     0.0f, 0.0f,

    // (y = 1)
	 -1.0f, 1.0f, -1.0f,      0.0f, 1.0f,
	  1.0f, 1.0f,  1.0f,      1.0f, 0.0f,
	  1.0f, 1.0f, -1.0f,      1.0f, 1.0f,
	  1.0f, 1.0f,  1.0f,      1.0f, 0.0f,
	 -1.0f, 1.0f, -1.0f,      0.0f, 1.0f,
	 -1.0f, 1.0f,  1.0f,      0.0f, 0.0f

};



unsigned int cubeVAO, cubeVBO;

// Initializes the cube's VAO and VBO and sets up the vertex position attribute.
void setupCube()
{
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	// position attribute	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

