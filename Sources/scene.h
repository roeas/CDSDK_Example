#pragma once

#include "mesh.h"

#include "Framework/IConsumer.h"
#include "Scene/SceneDatabase.h"
#include "Producers/CDProducer/CDProducer.h"
#include "Framework/Processor.h"
#include "GLConsumer.h"

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

class GLScene
{
public:
	GLScene() { m_pScene = new cd::SceneDatabase(); }

	cd::SceneDatabase *GetSene() { return m_pScene; }

	void LoadModel(const char *path);

	void SetShader(const Shader &shader) { m_shader = shader; }

	void Draw(const Shader &shader) const;

private:
	void DrawNode(const cd::Node &node) const;
	void DrawMesh(const cd::Node &node) const;

	cd::SceneDatabase *m_pScene;

	// Use it to manage the vertex buffer layout.
	// The remaining data can be obtained from the SceneDatabase.
	std::vector<GLMesh> m_meshes;

	Shader m_shader;
};
