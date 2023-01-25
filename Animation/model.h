#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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
#include "render_parameter.h"

class Model
{
public:
    Model(const string& model_path);

    void PlaySingleAnimation(const PlaySingleAnimParameter&);
    void BlendAnimation1D(const BlendAnimParameter&);
    void PlayAnimationTransition(const TransitionAnimParameter&);

    inline bool HaveAnimation() const;
    vector<string> GetAnimationNameList() const;
    vector<float> GetAnimationDurationList() const;

    const vector<mat4>& GetSkeletonTransformMatsRef() const;
    
    void Draw(const Shader& shader) const;

private:
    vector<Mesh> vec_mesh_;

    // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once
    vector<Texture> vec_loaded_tex_;
    // used to get texture path
    string model_directory_;

    vector<Animation*> vec_p_anims_;

    Skeleton* p_skeleton_ = nullptr;

    // root transform which are not affected by skeleton hierarchy
    // used to set transform for meshes that don't belong to skeleton
    mat4 root_transform_ = mat4(1.0f);
    mat4 GetModelRootTransform(const unordered_map<string, string>& node_parent, const unordered_map<string, mat4>& node_transform, const string& bone_root);

    void LoadModel(const string& path);

    void ProcessNode(const aiNode* node, const aiScene* scene, unordered_map<string, string>& node_parent, unordered_map<string, mat4>& node_transform);
    Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);

    void LoadAnimation(const aiScene* scene);

    vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

#endif