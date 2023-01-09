#pragma once

#include "Producers/CDProducer/CDProducer.h"
#include "Framework/Processor.h"
#include "GLConsumer.h"

class GLModel
{
public:
    GLModel(const char *path);
    void Draw(Shader &shader) const;

private:
    void loadModel(const char* path);

    std::vector<GLMesh> m_meshes;
};
