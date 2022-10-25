#ifndef MODEL_H
#define MODEL_H
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using glm::mat4;

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using std::string;
using std::vector;
using std::map;

#include "mesh.h"
#include "animation.h"
#include "skeleton.h"
#include "utility/anim_math.h"

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false){
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
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

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

class Model
{
public:
    // model data 
    vector<Mesh> vec_mesh_;
    vector<Texture> vec_loaded_tex_; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once
    string model_directory_; // used to get texture path

    vector<Animation*> vec_p_anims_;
    Skeleton* p_skeleton_ = nullptr;
    mat4 root_transform_ = mat4(1.0f); // root transform which are not affected by animation and skeleton

    
    Model(const string& model_path) {
        LoadModel(model_path);
    }

    void CalcBoneTransform(float time) {
        time *= 1000;
        //float anim_time = fmod(time * anim->mTicksPerSecond , anim->mDuration / anim->mTicksPerSecond);
        float anim_time = fmod(time, vec_p_anims_[0]->total_frames_);
        // p_skeleton_->CalcBoneAnimTransform(*vec_p_anims_[0], anim_time, root_transform_);
        p_skeleton_->BlendBoneAnimTransform(*vec_p_anims_[0], *vec_p_anims_[4], anim_time, 0.9, root_transform_);
    }

    void PlayAnimation(int anim_index, bool loop = true) {

    }

    void BlendAnimation(int anim_index1, int anim_index2, bool loop = true) {

    }

    void Draw(Shader& shader) const {
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "bones"), 75, GL_FALSE, glm::value_ptr(p_skeleton_->final_bone_transform_[0]));

        for (unsigned int i = 0; i < vec_mesh_.size(); i++)
        {
            vec_mesh_[i].Draw(shader);
        }
    }

private:
    void LoadModel(const string& path) {
        Assimp::Importer importer;
        const aiScene * scene = importer.ReadFile(path, aiProcess_Triangulate 
                                                                                 | aiProcess_GenSmoothNormals 
                                                                                 | aiProcess_FlipUVs 
                                                                                 | aiProcess_CalcTangentSpace
                                                                                 | aiProcess_LimitBoneWeights);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        // retrieve the directory path of the filepath
        model_directory_ = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively and store child to parent relation
        unordered_map<string, string> node_parent;
        unordered_map<string, mat4> node_transform; // used to get root transform
        ProcessNode(scene->mRootNode, scene, node_parent, node_transform);

        // skeleton has been loaded and store child to parent relation into skeleton
        if (p_skeleton_ != nullptr)
        {
            string bone_root = p_skeleton_->SetBoneChildToParent(node_parent);
            root_transform_ = GetModelRootTransform(node_parent, node_transform, bone_root);
        }

        // load animation data
        LoadAnimation(scene);
    }

    // get model root transform which are not affected by skeleton and animation
    mat4 GetModelRootTransform(const unordered_map<string, string>& node_parent, const unordered_map<string, mat4>& node_transform, const string& bone_root) {
        if (bone_root == string()) return mat4(1.0f);

        mat4 root_transform = mat4(1.0f);
        string curr_child = bone_root;
        while (node_parent.find(curr_child) != node_parent.end())
        {
            curr_child = node_parent.find(curr_child)->second;
            root_transform = node_transform.find(curr_child)->second * root_transform;
        }
        return root_transform;
    }


    // processes a node in a recursive fashion
    // processes each individual mesh located at the node and repeats this process on its children nodes (if any)
    // and store child to parent relation
    void ProcessNode(const aiNode* node, const aiScene* scene, unordered_map<string, string>& node_parent, unordered_map<string, mat4>& node_transform) {
        node_transform[node->mName.data] = Convert<mat4>(node->mTransformation);

        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            vec_mesh_.push_back(ProcessMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, node_parent, node_transform);
            node_parent[node->mChildren[i]->mName.data] = node->mName.data;
        }
    }


    Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene) {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;

            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }

            // texture coordinates
            // assume that each texture use the same tex coords
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                // tex coords
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.tex_coords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.bitangent = vector;
            }
            else
            {
                vertex.tex_coords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // vertex bone info
        if (mesh->HasBones())
        {
            // load bone
            if (p_skeleton_ == nullptr)
            {
                p_skeleton_ = new Skeleton();
            }
            p_skeleton_->LoadSkeletonAndRetrieveVertexInfo(mesh, vertices);
        }

        // vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texture_file; 
        material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file);

        if (const aiTexture* texture = scene->GetEmbeddedTexture(texture_file.C_Str()))
        {
            Texture text = Texture();
            unsigned int ID;
            glGenTextures(1, &ID);
            glBindTexture(GL_TEXTURE_2D, ID);

            text.id = ID;
            text.type = "texture_diffuse";

            unsigned char* image_data = nullptr;
            int width, height, components_per_pixel;
            if (texture->mHeight == 0)
            {
                image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &components_per_pixel, 0);
            }
            else
            {
                image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &components_per_pixel, 0);
            }

            if (components_per_pixel == 3)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
            }
            else if (components_per_pixel == 4)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);

            textures.push_back(text);
        }
        else
        {
            // 1. diffuse maps
            vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }


    void LoadAnimation(const aiScene* scene) {
        if (scene->HasAnimations() == false) return;

        for (int i = 0; i < scene->mNumAnimations; i++)
        {
            Animation* p_anim = new Animation(scene->mAnimations[i]);
            vec_p_anims_.push_back(p_anim);
        }
    }


    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
        vector<Texture> textures;

        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < vec_loaded_tex_.size(); j++)
            {
                if (std::strcmp(vec_loaded_tex_[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(vec_loaded_tex_[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }

            if (!skip)
            {   
                // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->model_directory_);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                vec_loaded_tex_.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }

        return textures;
    }
};

#endif