#include "skybox.h"

#include <iostream>

namespace
{
    const float SkyboxVertices[] =
    {
        -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f
    };

    const unsigned int SkyboxIndices[] =
    {
        //right face
        1, 2, 6, 
        6, 5, 1,

        //left face
        0, 4, 7, 
        7, 3, 0,

        //top face
        4, 5, 6, 
        6, 7, 4,

        //bottom face
        0, 3, 2, 
        2, 1, 0,

        //front face
        3, 7, 6, 
        6, 2, 3,

        //back face
        0, 1, 5, 
        5, 4, 0
    };

    const char* SkyboxFaces[6] =
    {
        "assets/skybox/galaxy_right.png",
        "assets/skybox/galaxy_left.png",
        "assets/skybox/galaxy_top.png",
        "assets/skybox/galaxy_bottom.png",
        "assets/skybox/galaxy_front.png",
        "assets/skybox/galaxy_back.png"
    };
}

Skybox::Skybox()
{
    m_vao = 0;
    m_vbo = 0;
    m_ebo = 0;
    m_texture = 0;
    m_shaderProg = 0;
    m_projectionMatrix = -1;
    m_viewMatrix = -1;
    m_sampler = -1;
}

Skybox::~Skybox()
{
    if (m_shaderProg != 0)
    {
        glDeleteProgram(m_shaderProg);
    }

    if (m_texture != 0)
    {
        glDeleteTextures(1, &m_texture);
    }

    if (m_ebo != 0)
    {
        glDeleteBuffers(1, &m_ebo);
    }

    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
    }

    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
    }
}

bool Skybox::Initialize()
{
    m_shaderProg = CreateSkyboxProgram();

    if (m_shaderProg == 0)
    {
        return false;
    }

    m_projectionMatrix = glGetUniformLocation(m_shaderProg, "projectionMatrix");
    m_viewMatrix = glGetUniformLocation(m_shaderProg, "viewMatrix");
    m_sampler = glGetUniformLocation(m_shaderProg, "skybox");
    if (m_projectionMatrix == -1 || m_viewMatrix == -1 || m_sampler == -1)
    {
        std::cerr << "Skybox uniform lookup failed." << std::endl;
        return false;
    }

    m_texture = LoadCubeMap(SkyboxFaces);

    if (m_texture == 0)
    {
        return false;
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertices), SkyboxVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SkyboxIndices), SkyboxIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    return true;
}

void Skybox::Render(const glm::mat4& projection, const glm::mat4& view)
{
    if (m_shaderProg == 0 || m_texture == 0 || m_vao == 0)
    {
        return;
    }

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glUseProgram(m_shaderProg);

    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(skyboxView));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
    glUniform1i(m_sampler, 0);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, sizeof(SkyboxIndices) / sizeof(SkyboxIndices[0]), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

GLuint Skybox::CompileShader(GLenum shaderType, const char* source)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader == 0)
    {
        return 0;
    }

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[1024] = { 0 };
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Skybox shader compile error: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint Skybox::CreateSkyboxProgram()
{
    const char* vertexSource =
        "#version 460\n"
        "layout (location = 0) in vec3 aPos;\n"
        "out vec3 TexCoords;\n"
        "uniform mat4 projectionMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "void main()\n"
        "{\n"
        "    TexCoords = aPos;\n"
        "    vec4 pos = projectionMatrix * viewMatrix * vec4(aPos, 1.0);\n"
        "    gl_Position = pos.xyww;\n"
        "}\n";

    const char* fragmentSource =
        "#version 460\n"
        "in vec3 TexCoords;\n"
        "out vec4 frag_color;\n"
        "uniform samplerCube skybox;\n"
        "void main()\n"
        "{\n"
        "    frag_color = texture(skybox, TexCoords);\n"
        "}\n";

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 || fragmentShader == 0)
    {
        if (vertexShader != 0) glDeleteShader(vertexShader);
        if (fragmentShader != 0) glDeleteShader(fragmentShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[1024] = { 0 };
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cerr << "Skybox shader link error: " << infoLog << std::endl;
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

GLuint Skybox::LoadCubeMap(const char* skyboxFaces[6])
{
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    for (unsigned int i = 0; i < 6; i++)
    {
        int width = 0;
        int height = 0;
        int channel = 0;

        unsigned char* data = SOIL_load_image(skyboxFaces[i], &width, &height, &channel, SOIL_LOAD_RGBA);
        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGBA,
                width,
                height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data
            );

            SOIL_free_image_data(data);
        }
        else
        {
            std::cout << "Failed to load texture " << skyboxFaces[i] << std::endl;
            glDeleteTextures(1, &textureID);
            return 0;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}