#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include <SOIL2/SOIL2.h>
#include "graphics_headers.h"

class Skybox
{
public:
    Skybox();
    ~Skybox();
    bool Initialize();
    void Render(const glm::mat4& projection, const glm::mat4& view);


private:
    GLuint CompileShader(GLenum shaderType, const char* source);
    GLuint CreateSkyboxProgram();
    GLuint LoadCubeMap(const char* skyboxFaces[6]);

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    GLuint m_texture;
    GLuint m_shaderProg;
    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_sampler;
};

#endif  /* SKYBOX_H */
