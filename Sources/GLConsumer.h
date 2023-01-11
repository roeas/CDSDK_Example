#pragma once

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stb_image.h>
#include <string>

#include "mesh.h"
#include "Framework/IConsumer.h"
#include "Scene/SceneDatabase.h"

class GLConsumer final : public cdtools::IConsumer
{
public:
	GLConsumer() = delete;
	explicit GLConsumer(std::string filePath) : m_filePath(cd::MoveTemp(filePath)) {}
	GLConsumer(const GLConsumer&) = delete;
	GLConsumer& operator=(const GLConsumer&) = delete;
	GLConsumer(GLConsumer&&) = delete;
	GLConsumer& operator=(GLConsumer&&) = delete;
	virtual ~GLConsumer() = default;

	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) override;

	std::vector<GLMesh>&& GetMeshes() { return cd::MoveTemp(m_meshes); }

private:
	std::string m_filePath;
	std::vector<GLMesh> m_meshes;
	std::map<std::string, GLTexture> m_textureLoaded;

	std::vector<GLTexture> LoadMaterialTextures(const cd::SceneDatabase* pSceneDatabase, const cd::Material& material, const cd::MaterialTextureType textureType);
	
	unsigned int TextureFromFile(const char* path, const std::string& directory);
};
