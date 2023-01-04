#include "mesh.h"

GLMesh::GLMesh(std::vector<GLVertex> &vertices, std::vector<unsigned int> &indices, std::vector<GLTexture> &textures) {
    this->vertices = std::move(vertices);
    this->indices = std::move(indices);
    this->textures = std::move(textures);

    setupMesh();
}

void GLMesh::Draw(Shader &shader) const {
    // bind texture
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    unsigned int reflectionNr = 1;
    for (unsigned int i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);

        cd::MaterialTextureType type = textures[i].type;
        if (type == cd::MaterialTextureType::BaseColor) {
            shader.setInt("s_texBaseColor", i);
        }
        else if (type == cd::MaterialTextureType::Normal) {
            shader.setInt("s_texNormal", i);
        }
        else if (type == cd::MaterialTextureType::Metalness) {
            shader.setInt("s_texORM", i);
        }

        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // Draw Elements
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void GLMesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLVertex), &vertices[0], GL_STATIC_DRAW);
    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void *)0);
    
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void *)offsetof(GLVertex, Normal));
    
    // UV
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void *)offsetof(GLVertex, TexCoords));
    
    // Tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void *)offsetof(GLVertex, Tangent));
    
    glBindVertexArray(0);
}
