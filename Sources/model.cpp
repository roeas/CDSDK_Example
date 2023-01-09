#include "model.h"

GLModel::GLModel(const char* path) {
    loadModel(path);
}

void GLModel::Draw(Shader &shader) const {
    for (unsigned int i = 0; i < m_meshes.size(); ++i) {
        m_meshes[i].Draw(shader);
    }
}

void GLModel::loadModel(const char* path) {
    cdtools::CDProducer producer(path);
    GLConsumer consumer("");
    cdtools::Processor processor(&producer, &consumer);
    processor.Run();
    m_meshes = consumer.GetMeshes();
}

