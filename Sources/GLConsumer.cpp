#include "GLConsumer.h"

constexpr cd::MaterialTextureType PossibleTextureTypes[] = {
	cd::MaterialTextureType::BaseColor,
	cd::MaterialTextureType::Normal,
	cd::MaterialTextureType::Metalness,
};

void GLConsumer::Execute(const cd::SceneDatabase* pSceneDatabase) {
	printf("Loading scene : %s\n", pSceneDatabase->GetName());
	const std::vector<cd::Mesh>& meshes = pSceneDatabase->GetMeshes();
	printf("Mesh count : %llu\n", meshes.size());

	for (uint32_t meshIndex = 0; meshIndex < pSceneDatabase->GetMeshCount(); ++meshIndex) {
		const cd::Mesh& mesh = meshes[meshIndex];
		printf("\tMeshName : %s\n", mesh.GetName());
		printf("\t\tVertexCount : %u\n", mesh.GetVertexCount());
		printf("\t\tPolygonCount : %u\n\n", mesh.GetPolygonCount());

		std::vector<GLVertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<GLTexture> textures;

		// 1. vertices
		vertices.reserve(mesh.GetVertexCount());
		for (uint32_t vertexIndex = 0; vertexIndex < mesh.GetVertexCount(); ++vertexIndex) {
			const cd::Point& position = mesh.GetVertexPosition(vertexIndex);
			const cd::Direction& normal = mesh.GetVertexNormal(vertexIndex);
			const cd::Direction& tangent = mesh.GetVertexTangent(vertexIndex);
			const cd::UV& uv = mesh.GetVertexUV(0)[vertexIndex];

			GLVertex vertex;
			std::memcpy(&vertex.Position, &position, 3 * sizeof(float));
			std::memcpy(&vertex.Normal, &normal, 3 * sizeof(float));
			std::memcpy(&vertex.Tangent, &tangent, 3 * sizeof(float));
			std::memcpy(&vertex.TexCoords, &uv, 2 * sizeof(float));
			
			vertices.emplace_back(std::move(vertex));
		}

		// 2. indices
		indices.reserve(mesh.GetPolygonCount() * 3);
		for (uint32_t i = 0; i < mesh.GetPolygonCount(); ++i) {
			indices.push_back(mesh.GetPolygon(i)[0].Data());
			indices.push_back(mesh.GetPolygon(i)[1].Data());
			indices.push_back(mesh.GetPolygon(i)[2].Data());
		}

		// 3. textures
		const cd::Material& material = pSceneDatabase->GetMaterial(meshIndex);
		for (const cd::MaterialTextureType& textureType : PossibleTextureTypes) {
			std::vector<GLTexture> typeTextures = loadMaterialTextures(pSceneDatabase, material, textureType);
			textures.insert(textures.end(), typeTextures.begin(), typeTextures.end());
		}

		m_meshes.emplace_back(GLMesh(vertices, indices, textures));
	}
}

std::vector<GLTexture> GLConsumer::loadMaterialTextures(const cd::SceneDatabase* pSceneDatabase, const cd::Material& material, const cd::MaterialTextureType textureType) {
	std::vector<GLTexture> textures;

	const std::optional<cd::TextureID>& textureID = material.GetTextureID(textureType);
	if (textureID.has_value()) {
		const std::string& texturePath = pSceneDatabase->GetTexture(textureID->Data()).GetPath();
		std::string textureName = texturePath.substr(texturePath.rfind('/') + 1, texturePath.rfind('.') - texturePath.rfind('/') - 1);
		printf("\t\tTexture Name: %s\n", textureName.c_str());

		const auto it = m_textureLoaded.find(textureName);
		if (it != m_textureLoaded.end()) {
			textures.push_back(it->second);
		}
		else {
			GLTexture texture;
			texture.id = TextureFromFile(texturePath.c_str(), "Models");
			texture.type = textureType;
			texture.path = texturePath;
			m_textureLoaded[textureName] = texture;
			textures.emplace_back(std::move(texture));
		}
	}
	else {
		printf("\t\tTexture Name: UnknownMaterial\n");
	}

	return textures;
}

unsigned int GLConsumer::TextureFromFile(const char* path, const std::string& directory) {
	std::string filename(path);
	filename = directory + '/' + filename;
	printf("\t\tTexture Path: %s\n\n", filename.c_str());

	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format = GL_RGBA;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		printf("\t\tTexture failed to load at path: %s\n\n", filename.c_str());
	}
	stbi_image_free(data);

	return textureID;
}