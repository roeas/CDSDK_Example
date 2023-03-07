#include "scene.h"

void GLScene::LoadModel(const char *path) {
	cdtools::CDProducer producer(path);
	GLConsumer consumer("");

	cdtools::Processor processor(&producer, &consumer, m_pScene);
	processor.Run();

	m_meshes = consumer.GetMeshes();
}

void GLScene::Draw(const Shader &shader) const {
	const cd::Node &Root = m_pScene->GetNode(0);
	DrawNode(Root);
}

void GLScene::DrawNode(const cd::Node &node) const {
	if(node.GetChildCount()) {
		for(const auto &childID : node.GetChildIDs()) {
			const cd::Node &childNode = m_pScene->GetNode(childID.Data());

			// const auto &trans = childNode.GetTransform();
			// m_shader.SetMat3(trans);

			DrawMesh(childNode);
			DrawNode(childNode);
		}
	}
}

void GLScene::DrawMesh(const cd::Node &node) const {
	if(node.GetMeshCount()) {
		for(const auto &meshID : node.GetMeshIDs()) {
			assert(meshID.Data() < m_meshes.size());
			m_meshes[meshID.Data()].Draw(m_shader);
		}
	}
}
