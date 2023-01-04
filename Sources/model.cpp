#include "model.h"

GLModel::GLModel(std::string const &path, bool gamma) {
    loadModel(path);
}

void GLModel::Draw(Shader &shader) const {
    for (unsigned int i = 0; i < m_meshes.size(); ++i) {
        m_meshes[i].Draw(shader);
    }
}

void GLModel::loadModel(std::string const &path) {
    cdtools::CDProducer producer(path.c_str());
    GLConsumer consumer("");
    cdtools::Processor processor(&producer, &consumer);
    processor.Run();
    m_meshes = consumer.GetMeshes();
}

