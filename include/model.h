#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>

#include <shader.h>
#include <mesh.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

using namespace std;

class Model
{
public:
    Model(const std::filesystem::path& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    void Draw(Shader &shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            meshes[i].Draw(shader);
        }
    }

private:
    vector<Mesh> meshes;
    vector<Texture> texturesLoaded;
    string directory;
    bool gammaCorrection;

    void loadModel(const std::filesystem::path& path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs |
                                                           aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes);

        if (!scene || scene->mFlags * AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "Error::ASSIMP::" << importer.GetErrorString() << endl;
            return;
        }

        // if it works get parent directory to load textures from.
		directory = path.parent_path().string();
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene)
    {
        // iterate all meshes in the node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // pull single mesh out of node's mesh array
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

            // process the mesh and add it to the array of MY meshes
            meshes.push_back(processMesh(mesh, scene));
        }

        // Now process all children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    // vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    // unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            // VERTEX (position, normal, tangent, bitangent, and texCoords)
            Vertex vertex;
            glm::vec3 position;
            // tmp variable cuz I'm lazy and this is a lil faster I think
            aiVector3D tmp = mesh->mVertices[i];
            position.x = tmp.x;
            position.y = tmp.y;
            position.z = tmp.z;
            vertex.Position = position;

            if (mesh->HasNormals())
            {
                tmp = mesh->mNormals[i];
                glm::vec3 normal;
                normal.x = tmp.x;
                normal.y = tmp.y;
                normal.z = tmp.z;
                vertex.Normal = normal;
            }

            if (mesh->HasTangentsAndBitangents())
            {
                tmp = mesh->mTangents[i];
                glm::vec3 tangent;
                tangent.x = tmp.x;
                tangent.y = tmp.y;
                tangent.z = tmp.z;
                vertex.Tangent = tangent;

                tmp = mesh->mBitangents[i];
                glm::vec3 bitangent;
                bitangent.x = tmp.x;
                bitangent.y = tmp.y;
                bitangent.z = tmp.z;
                vertex.Bitangent = bitangent;
            }

            if (mesh->mTextureCoords[0])
            {
                // can have 8 coords only using 2
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;

                vertex.TexCoords = vec;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        // INDICES
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // TEXTURES
        if (mesh->mMaterialIndex >= 0)
        {
            // Get material, then get respective textures
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }
        return Mesh(vertices, indices, textures);
    }

    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            Texture texture;
            bool skip = false;
            for (unsigned int j = 0; j < texturesLoaded.size(); j++)
            {
                if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(texturesLoaded[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.path = str.C_Str();
                texture.type = typeName;
                textures.push_back(texture);
                texturesLoaded.push_back(texture);
            }
        }
        return textures;
    }

    unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false)
    {
        string fileName = string(path);
        fileName = directory + '/' + fileName;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format = GL_RED;
            switch (nrComponents)
            {
                case 1:
				    format = GL_RED;
                    break;
                case 2:
                    format = GL_RG;
					break;
                case 3:
                    format = GL_RGB;
                    break;
                case 4:
                    format = GL_RGBA;
					break;
                default:
                    std::cout << "Texture failed to load::Unknown number of components: " << nrComponents << std::endl;
					stbi_image_free(data);
                    return textureID;
            }
            

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
};
#endif