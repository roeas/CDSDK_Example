#include "mesh.h"

GLMesh::GLMesh(std::vector<GLVertex> &vertices, std::vector<unsigned int> &indices, std::vector<GLTexture> &textures) {
    this->m_vertices = std::move(vertices);
    this->m_indices = std::move(indices);
    this->m_textures = std::move(textures);

    SetupMesh();
}

void GLMesh::Draw(Shader &shader) const {
    // bind texture
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    unsigned int reflectionNr = 1;
    for (unsigned int i = 0; i < m_textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);

        cd::MaterialTextureType type = m_textures[i].m_type;
        if (type == cd::MaterialTextureType::BaseColor) {
            shader.SetInt("s_texBaseColor", i);
        }
        else if (type == cd::MaterialTextureType::Normal) {
            shader.SetInt("s_texNormal", i);
        }
        else if (type == cd::MaterialTextureType::Metalness) {
            shader.SetInt("s_texORM", i);
        }

        glBindTexture(GL_TEXTURE_2D, m_textures[i].m_id);
    }

    // Draw Elements
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void GLMesh::SetupMesh() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLVertex), &m_vertices[0], GL_STATIC_DRAW);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void *)0);
    
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void *)offsetof(GLVertex, m_normal));
    
    // UV
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void *)offsetof(GLVertex, m_texCoords));
    
    // Tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void *)offsetof(GLVertex, m_tangent));
    
    glBindVertexArray(0);
}
