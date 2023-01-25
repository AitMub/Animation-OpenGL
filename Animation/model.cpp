#include "model.h"

#include "utility/file_loader.h"
#include "utility/anim_math.h"

#include <stb_image.h>
#define STB_IMAGE_IMPLEMENTATION


Model::Model(const string& model_path) {
    LoadModel(model_path);
}

void Model::PlaySingleAnimation(const PlaySingleAnimParameter& parameter) {
    p_skeleton_->CalcBoneAnimTransform(*vec_p_anims_[parameter.anim_index], parameter.normalized_time, root_transform_);
}

void Model::BlendAnimation1D(const BlendAnimParameter& parameter) {
    p_skeleton_->BlendBoneAnimTransform(*vec_p_anims_[parameter.anim_index1], *vec_p_anims_[parameter.anim_index2], 
        parameter.normalized_time, parameter.anim_blend_weight, root_transform_);
}

void Model::PlayAnimationTransition(const TransitionAnimParameter& parameter) {
    p_skeleton_->TransitionAnim(*vec_p_anims_[parameter.anim_index1], *vec_p_anims_[parameter.anim_index2], 
        parameter.time_in_sec, parameter.begin_trans_time_in_sec, root_transform_);
}

bool Model::HaveAnimation() const {
    return vec_p_anims_.size() > 0;
}

vector<string> Model::GetAnimationNameList() const {
    if (HaveAnimation() == false)
    {
        return vector<string>();
    }

    vector<string> animation_name_list;
    for (int i = 0; i < vec_p_anims_.size(); i++)
    {
        animation_name_list.emplace_back(vec_p_anims_[i]->anim_name_);
    }
    return animation_name_list;
}

vector<float> Model::GetAnimationDurationList() const {
    if (HaveAnimation() == false)
    {
        return vector<float>();
    }

    vector<float> animation_duration_list;
    for (int i = 0; i < vec_p_anims_.size(); i++)
    {
        animation_duration_list.emplace_back(vec_p_anims_[i]->total_sec_);
    }
    return animation_duration_list;
}

const vector<mat4>& Model::GetSkeletonTransformMatsRef() const {
    return p_skeleton_->GetFinalBoneTransform();
}

void Model::Draw(const Shader& shader) const {
    for (unsigned int i = 0; i < vec_mesh_.size(); i++)
    {
        vec_mesh_[i].Draw(shader);
    }
}

void Model::LoadModel(const string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate
        | aiProcess_GenSmoothNormals
        | aiProcess_FlipUVs
        | aiProcess_CalcTangentSpace
        | aiProcess_LimitBoneWeights);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
    {
        throw string("Assimp Error:") + importer.GetErrorString();
    }

    // retrieve the directory path of the filepath
    model_directory_ = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively and store child to parent relation
    unordered_map<string, string> node_parent;
    // used to get root transform
    unordered_map<string, mat4> node_transform;
    ProcessNode(scene->mRootNode, scene, node_parent, node_transform);

    // skeleton has been loaded and store child to parent relation into skeleton
    if (p_skeleton_ != nullptr)
    {
        p_skeleton_->SetBoneChildToParent(node_parent);
        Bone root_bone = p_skeleton_->GetRootBone();
        root_transform_ = GetModelRootTransform(node_parent, node_transform, root_bone.name);
    }

    LoadAnimation(scene);
}


void Model::ProcessNode(const aiNode* node, const aiScene* scene, unordered_map<string, string>& node_parent, unordered_map<string, mat4>& node_transform) {
    node_transform[node->mName.data] = Convert<mat4>(node->mTransformation);

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


Mesh Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene) {
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    Vertex temp_vertex;
    glm::vec3 temp_vec3;
    glm::vec2 temp_vec2;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        temp_vec3.x = mesh->mVertices[i].x;
        temp_vec3.y = mesh->mVertices[i].y;
        temp_vec3.z = mesh->mVertices[i].z;
        temp_vertex.position = temp_vec3;

        if (mesh->HasNormals())
        {
            temp_vec3.x = mesh->mNormals[i].x;
            temp_vec3.y = mesh->mNormals[i].y;
            temp_vec3.z = mesh->mNormals[i].z;
            temp_vertex.normal = temp_vec3;
        }

        // assume that each texture use the same tex coords
        if (mesh->mTextureCoords[0])
        {
            // tex coords
            temp_vec2.x = mesh->mTextureCoords[0][i].x;
            temp_vec2.y = mesh->mTextureCoords[0][i].y;
            temp_vertex.tex_coords = temp_vec2;
            // tangent
            temp_vec3.x = mesh->mTangents[i].x;
            temp_vec3.y = mesh->mTangents[i].y;
            temp_vec3.z = mesh->mTangents[i].z;
            temp_vertex.tangent = temp_vec3;
            // bitangent
            temp_vec3.x = mesh->mBitangents[i].x;
            temp_vec3.y = mesh->mBitangents[i].y;
            temp_vec3.z = mesh->mBitangents[i].z;
            temp_vertex.bitangent = temp_vec3;
        }
        else
        {
            temp_vertex.tex_coords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(temp_vertex);
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

    if (const aiTexture* texture_in_mem = scene->GetEmbeddedTexture(texture_file.C_Str()))
    {
        Texture texture = LoadTextureFromMemory(texture_in_mem);
        textures.push_back(texture);
    }
    else
    {
        vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    // return would call automatically generated move constructor
    // so it doesn't need to reallocate memory
    return Mesh(vertices, indices, textures);
}


// checks all material textures of a given type and loads the textures if they're not loaded yet
// the required info is returned as a Texture struct
vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
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
                skip = true; // a texture with the same filepath has already been loaded, continue to next one
                break;
            }
        }

        if (!skip)
        {
            // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = LoadTextureFromFile(str.C_Str(), this->model_directory_);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            vec_loaded_tex_.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }

    return textures;
}


mat4 Model::GetModelRootTransform(const unordered_map<string, string>& node_parent, const unordered_map<string, mat4>& node_transform, const string& bone_root) {
    if (bone_root == string()) return mat4(1.0f);

    mat4 root_transform = mat4(1.0f);
    string curr_child = bone_root;

    // from bone's root all the way up to model's root
    while (node_parent.find(curr_child) != node_parent.end())
    {
        curr_child = node_parent.find(curr_child)->second;
        root_transform = node_transform.find(curr_child)->second * root_transform;
    }
    return root_transform;
}


void Model::LoadAnimation(const aiScene* scene) {
    if (scene->HasAnimations() == false) return;

    for (int i = 0; i < scene->mNumAnimations; i++)
    {
        Animation* p_anim = new Animation(scene->mAnimations[i]);
        vec_p_anims_.push_back(p_anim);
    }
}