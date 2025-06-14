#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // Tangent
    glm::vec3 Tangent;
    // Bitangent
    glm::vec3 Bitangent;
    // TexCoords
    glm::vec2 TexCoords;

    // Bones I dunno
    int m_boneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
    unsigned int id;
    string type;
    string path;
};

class Mesh
{
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // Constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        SetupMesh();
    }
    // Render Command
    void Draw(Shader &shader)
    {
        // wanna bind textures first then render

        unsigned int diffuseNR = 1;
        unsigned int specularNR = 1;
        unsigned int normalNR = 1;
        unsigned int heightNR = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            // bind correct texture spot in memory
            glActiveTexture(GL_TEXTURE0 + i);

            string number;
            string name = textures[i].type;

            if (name == "texture_diffuse")
                number = std::to_string(diffuseNR++);
            if (name == "texture_specular")
                number = std::to_string(specularNR++);
            if (name == "texture_normal")
                number = std::to_string(normalNR++);
            if (name == "texture_height")
                number = std::to_string(heightNR++);

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // DRAW
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Make 0 the active texture again
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // Render variables
    unsigned int VAO, VBO, EBO;
    void SetupMesh()
    {
        // Generate the array object and buffers for vertices and elements.
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // Bind the vertex buffer into context (and also the array object)
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Supply data to vertex buffer. Can leverage struct memory architecture
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // Bind the element buffer into context
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // Supply data to elem buffer
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Now supply array object with buffers
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

        // Vertex Normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));

        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));

        // Vertex Tangents
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));

        // Vertex Bitangents
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));

        // Bone IDs
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, MAX_BONE_INFLUENCE, GL_INT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_boneIDs));

        // Bone Weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));

        // Bind array Object to 0
        glBindVertexArray(0);
    }
};

#endif