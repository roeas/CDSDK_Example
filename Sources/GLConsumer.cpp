#include "GLConsumer.h"

constexpr cd::MaterialTextureType PossibleTextureTypes[] = {
	cd::MaterialTextureType::BaseColor,
	cd::MaterialTextureType::Normal,
	cd::MaterialTextureType::Metalness,
};

void GLConsumer::Execute(const cd::SceneDatabase* pSceneDatabase) {
	printf("Loading scene : %s\n", pSceneDatabase->GetName());
	printf("Mesh count : %d\n", pSceneDatabase->GetMeshCount());
	printf("Material count : %d\n", pSceneDatabase->GetMaterialCount());
	printf("Texture count : %d\n", pSceneDatabase->GetTextureCount());
	printf("Light count : %d\n", pSceneDatabase->GetLightCount());
	const cd::AABB& sceneAABB = pSceneDatabase->GetAABB();
	printf("Scene AABB min : (%f, %f, %f), max : (%f, %f, %f)\n",
		sceneAABB.Min().x(), sceneAABB.Min().y(), sceneAABB.Min().z(),
		sceneAABB.Max().x(), sceneAABB.Max().y(), sceneAABB.Max().z());

	const std::vector<cd::Mesh>& meshes = pSceneDatabase->GetMeshes();
	for (uint32_t meshIndex = 0; meshIndex < pSceneDatabase->GetMeshCount(); ++meshIndex) {
		const cd::Mesh& mesh = meshes[meshIndex];
		printf("\n\tMesh ID : %d\n", mesh.GetID().Data());

		printf("\t\tMesh Name : %s\n", mesh.GetName());
		printf("\t\tVertex Count : %d\n", mesh.GetVertexCount());
		printf("\t\tPolygon Count : %d\n", mesh.GetPolygonCount());

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
			memcpy(&vertex.m_position, &position, 3 * sizeof(float));
			memcpy(&vertex.m_normal, &normal, 3 * sizeof(float));
			memcpy(&vertex.m_tangent, &tangent, 3 * sizeof(float));
			memcpy(&vertex.m_texCoords, &uv, 2 * sizeof(float));

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
		const cd::MaterialID& materialID = mesh.GetMaterialID();
		printf("\n\t\tMaterial ID : %d\n", materialID.Data());
		const cd::Material& material = pSceneDatabase->GetMaterial(materialID.Data());
		assert(materialID.Data() == material.GetID().Data());

		printf("\t\t\tMaterial name : %s\n\n", material.GetName());
		for (const cd::MaterialTextureType& textureType : PossibleTextureTypes) {
			std::vector<GLTexture> typeTextures = LoadMaterialTextures(pSceneDatabase, material, textureType);
			textures.insert(textures.end(), typeTextures.begin(), typeTextures.end());
		}

		m_meshes.emplace_back(GLMesh(vertices, indices, textures));
	}
}

std::vector<GLTexture> GLConsumer::LoadMaterialTextures(const cd::SceneDatabase* pSceneDatabase, const cd::Material& material, const cd::MaterialTextureType textureType) {
	std::vector<GLTexture> textures;

	const std::optional<cd::TextureID>& textureID = material.GetTextureID(textureType);
	if (textureID.has_value()) {
		const std::string& texturePath = pSceneDatabase->GetTexture(textureID->Data()).GetPath();
		std::string textureName = texturePath.substr(texturePath.rfind('/') + 1, texturePath.rfind('.') - texturePath.rfind('/') - 1);
		printf("\t\t\tTexture Name: %s\n", textureName.c_str());

		const auto it = m_textureLoaded.find(textureName);
		if (it != m_textureLoaded.end()) {
			textures.push_back(it->second);
		}
		else {
			GLTexture texture;
			texture.m_id = TextureFromFile(texturePath.c_str(), "Models");
			texture.m_type = textureType;
			texture.m_path = texturePath;
			m_textureLoaded[textureName] = texture;
			textures.emplace_back(std::move(texture));
		}
	}
	else {
		printf("\t\t\tTexture Name: UnknownMaterial\n");
	}

	return textures;
}

unsigned int GLConsumer::TextureFromFile(const char* path, const std::string& directory) {
	std::string filename(path);
	filename = directory + '/' + filename;
	printf("\t\t\t[Read File] Texture Path: %s\n", filename.c_str());

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
		printf("\t\t\tTexture failed to load at path: %s\n\n", filename.c_str());
	}
	stbi_image_free(data);

	return textureID;
}
