#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Vertex {
public:
    Vertex(float vertices[], int verSize, unsigned int indices[], int indSize) {
        unsigned int VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verSize, vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indices, GL_STATIC_DRAW);
    }


    void setProperty(int pId, int pSize, int vSize, int pOffset) {
        glVertexAttribPointer(pId, pSize, GL_FLOAT, GL_FALSE, vSize * sizeof(float), (void *)(pOffset * sizeof(float)));
        glEnableVertexAttribArray(pId);
    }
};
