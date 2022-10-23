#ifndef SKELETON_H
#define SKELETON_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using glm::mat4;

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
using std::string;
using std::vector;
using std::unordered_map;

#include "mesh.h"
#include "animation.h"

struct Bone
{
	const mat4 offset;
	mat4 transform;
	int parent_index;
	string name;

	Bone(const mat4& _offset, const mat4& _transform, const int& _parent_index, const string& _name) :
		offset(_offset), transform(_transform), parent_index(_parent_index), name(_name) {}
};

class Skeleton
{
public:
	Skeleton();

	void LoadSkeletonAndRetrieveVertexInfo(const aiMesh* const mesh, vector<Vertex>& vertices);
	string SetBoneChildToParent(const unordered_map<string, string>& node_parent);

	void CalcBoneAnimTransform(const Animation& animation, float time, const mat4& root_transform);

	vector<mat4> final_bone_transform_;
private:
	vector<Bone> vec_bone_;
	int bone_nums_;
	unordered_map<string, int> bone_name_to_index_;

	void SetVertexBoneInfo(Vertex& vertex, unsigned int bone_index, float weight) const;
};

#endif