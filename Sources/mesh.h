#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "Scene/SceneDatabase.h"

struct GLVertex {
    glm::vec3 m_position;
    glm::vec3 m_normal;
    glm::vec2 m_texCoords;
    glm::vec3 m_tangent;
};

struct GLTexture {
    unsigned int m_id;
    cd::MaterialTextureType m_type;
    std::string m_path;
};

class GLMesh {
public:
    GLMesh(std::vector<GLVertex> &vertices, std::vector<unsigned int> &indices, std::vector<GLTexture> &textures);

    void Draw(Shader &shader) const;

    std::vector<GLVertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<GLTexture> m_textures;
    unsigned int m_VAO;

private:
    void SetupMesh();
    
    unsigned int m_VBO, m_EBO;
};
