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
#include <map>
#include <vector>
using std::string;
using std::vector;
using std::map;

class Skeleton
{
public:
	void LoadSkeletonAndRetrieveVertexInfo(const aiMesh* const mesh, vector<Vertex>& vec_vertex_info);

private:
	vector<mat4> vec_bone_offset_;
	vector<mat4> vec_bone_transform_;
	map<string, int> bone_name_to_index_;
};

#endif