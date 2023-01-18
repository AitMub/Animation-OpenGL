#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "shader.h"

// could change to constexpr?
const int kMaxBonePerVertex = 4;

struct Vertex 
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    //bone indexes which will influence this vertex
    int bone_id[kMaxBonePerVertex];
    //weights from each bone
    float weights[kMaxBonePerVertex];

    Vertex() : position(0.0f), normal(0.0f), tex_coords(0.0f), tangent(0.0f), bitangent(0.0f) {
        std::fill_n(bone_id, kMaxBonePerVertex, -1);
        std::fill_n(weights, kMaxBonePerVertex, -1.0f);
    }
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
    // mesh Data
    vector<Vertex>       vertices_;
    vector<unsigned int> indices_;
    vector<Texture>      textures_;
    unsigned int VAO_;

    // should change to rvalue ctor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
        vertices_ = vertices;
        indices_ = indices;
        textures_ = textures;

        SetupMesh();
    }

    // render the mesh
    void Draw(const Shader& shader) const {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;

        for (unsigned int i = 0; i < textures_.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures_[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);
            else if (name == "texture_height")
                number = std::to_string(heightNr++);

            // set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // bind the texture
            glBindTexture(GL_TEXTURE_2D, textures_[i].id);
        }

        // draw mesh
        glBindVertexArray(VAO_);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices_.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // render data 
    unsigned int VBO_, EBO_;

    // initializes all the buffer objects/arrays
    void SetupMesh() {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);
        glGenBuffers(1, &EBO_);

        glBindVertexArray(VAO_);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
        // bone ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, kMaxBonePerVertex, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, bone_id));
        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, kMaxBonePerVertex, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
        glBindVertexArray(0);
    }
};
#endif
