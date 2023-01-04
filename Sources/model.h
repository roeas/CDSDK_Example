#pragma once

#include "Producers/CDProducer/CDProducer.h"
#include "Framework/Processor.h"
#include "GLConsumer.h"

class GLModel
{
public:
    GLModel(std::string const &path, bool gamma = false);
    void Draw(Shader &shader) const;

private:
    void loadModel(std::string const &path);

    std::vector<GLMesh> m_meshes;
};
