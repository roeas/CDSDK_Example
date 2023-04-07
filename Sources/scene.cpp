#include "scene.h"

void GLScene::LoadModel(const char *path) {
	cdtools::CDProducer producer(path);
	GLConsumer consumer("");

	cdtools::Processor processor(&producer, &consumer, m_pScene);
	processor.Run();

	m_meshes = consumer.GetMeshes();
}

void GLScene::Draw(const Shader &shader) const {
	for(const auto &mesh : m_meshes) {
		mesh.Draw(shader);
	}
}
