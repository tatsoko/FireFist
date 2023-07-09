//
//  CameraRenderer.cpp
//  FireFist
//
//  Created by Administrator on 09.07.23.
//

#include "CameraRenderer.hpp"


CameraRenderer::CameraRenderer(Shader &shader)
{
    this->shader = shader;
    //this->initRenderData();
}

CameraRenderer::~CameraRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void CameraRenderer::DrawCamera(cv::Mat frame)
{
    // prepare transformations
    this->shader.Use();
    // configure VAO/VBO
    unsigned int VBO;
    
    float vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f
    };

    GLfloat texCoords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glBindVertexArray(this->quadVAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(texCoords), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(texCoords), texCoords);
    
    // Set up vertex attributes
    GLint positionAttrib = glGetAttribLocation(shader.ID, "position");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLint texCoordAttrib = glGetAttribLocation(shader.ID, "texCoord");
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(vertices));

    // Create and bind texture
    GLuint camera_texture;
    glGenTextures(1, &camera_texture);
    glBindTexture(GL_TEXTURE_2D, camera_texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the image data into the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.ptr());

    // Set uniform for texture sampler
    GLint textureSamplerUniform = glGetUniformLocation(shader.ID, "textureSampler");
    glUniform1i(textureSamplerUniform, 0);

    
    
    // Render the quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // Cleanup
    glDeleteTextures(1, &camera_texture);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &this->quadVAO);
}
