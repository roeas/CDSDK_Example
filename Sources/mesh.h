#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "Scene/SceneDatabase.h"

struct GLVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
};

struct GLTexture {
    unsigned int id;
    cd::MaterialTextureType type;
    std::string path;
};

class GLMesh {
public:
    std::vector<GLVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<GLTexture> textures;
    unsigned int VAO;

    GLMesh(std::vector<GLVertex> &vertices, std::vector<unsigned int> &indices, std::vector<GLTexture> &textures);

    void Draw(Shader &shader) const;

private:
    unsigned int VBO, EBO;

    void setupMesh();
};
