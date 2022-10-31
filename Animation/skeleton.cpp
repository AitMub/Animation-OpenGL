#include"skeleton.h"
#include"utility/anim_math.h"

Skeleton::Skeleton() :
	bone_nums_(0),
	vec_bone_(),
	bone_name_to_index_() {}


void Skeleton::LoadSkeletonAndRetrieveVertexInfo(const aiMesh* const mesh, vector<Vertex>& vertices) {
	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		unsigned int bone_index = 0;
		string bone_name = mesh->mBones[i]->mName.data;

		if (bone_name_to_index_.find(bone_name) == bone_name_to_index_.end())
		{
			bone_index = bone_nums_++;
			aiMatrix4x4 ai_mat_bone_offset = mesh->mBones[i]->mOffsetMatrix;
			vec_bone_.emplace_back(Convert<mat4>(ai_mat_bone_offset), mat4(1.0f), -1, bone_name);
			bone_name_to_index_[bone_name] = bone_index;
		}
		else
		{
			bone_index = bone_name_to_index_[bone_name];
		}

		// 为这个bone影响的所有顶点设置权重数据
		for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			unsigned int vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			SetVertexBoneInfo(vertices[vertex_id], bone_index, weight);
		}
	}
}

string Skeleton::SetBoneChildToParent(const unordered_map<string, string>& node_parent) {
	for (const auto& iter : node_parent)
	{
		if (bone_name_to_index_.find(iter.first) != bone_name_to_index_.end()
			&& bone_name_to_index_.find(iter.second) != bone_name_to_index_.end())
		{
			vec_bone_[bone_name_to_index_[iter.first]].parent_index = bone_name_to_index_[iter.second];
		}
	}

	// return bone root
	for (const auto& bone : vec_bone_)
	{
		if (bone.parent_index == -1)
		{
			return bone.name;
		}
	}
	return string();
}

void Skeleton::CalcBoneAnimTransform(const Animation& animation, float normalized_time, const mat4& root_transform) {
	// calculate hierarchy transform
	for (int i = 0; i < vec_bone_.size(); i++)
	{
		mat4 pos = glm::translate(mat4(1.0f), animation.GetPosition(vec_bone_[i].name, normalized_time));
		mat4 rot = glm::mat4_cast(animation.GetRotation(vec_bone_[i].name, normalized_time));
		// mat4 scale = glm::scale(mat4(1.0f), vec3(animation.GetScale(vec_bone_[i].name, time)));

		int parent_i = vec_bone_[i].parent_index;
		// check if this bone have parent
		mat4 parent_mat = parent_i >= 0 ? vec_bone_[parent_i].transform : root_transform;
		vec_bone_[i].transform = parent_mat * pos * rot;
	}

	// calculate final transform
	final_bone_transform_.resize(vec_bone_.size());
	for (int i = 0; i < vec_bone_.size(); i++)
	{
		final_bone_transform_[i] = vec_bone_[i].transform * vec_bone_[i].offset;
	}
}

void Skeleton::TransitionAnim(const Animation& anim1, const Animation& anim2, float trans_begin_norm_time, float normalized_time, const mat4& root_transform) {
	normalized_time = fmod(normalized_time, 1.0f + trans_begin_norm_time);

	if (normalized_time <= trans_begin_norm_time)
	{
		CalcBoneAnimTransform(anim1, normalized_time, root_transform);
	}
	else if (normalized_time > trans_begin_norm_time && normalized_time <= 1)
	{
		for (int i = 0; i < vec_bone_.size(); i++)
		{
			mat4 pos1 = glm::translate(mat4(1.0f), anim1.GetPosition(vec_bone_[i].name, normalized_time, true));
			mat4 rot1 = glm::mat4_cast(anim1.GetRotation(vec_bone_[i].name, normalized_time, true));

			mat4 pos2 = glm::translate(mat4(1.0f), anim2.GetPosition(vec_bone_[i].name, normalized_time - trans_begin_norm_time, true));
			mat4 rot2 = glm::mat4_cast(anim2.GetRotation(vec_bone_[i].name, normalized_time - trans_begin_norm_time, true));

			float weight = (normalized_time - trans_begin_norm_time) / (1.0f - trans_begin_norm_time);

			pos1 = Interpolate(pos1, pos2, weight);
			rot1 = Interpolate(rot1, rot2, weight);

			int parent_i = vec_bone_[i].parent_index;
			// check if this bone have parent
			mat4 parent_mat = parent_i >= 0 ? vec_bone_[parent_i].transform : root_transform;
			vec_bone_[i].transform = parent_mat * pos1 * rot1;
		}

		// calculate final transform
		final_bone_transform_.resize(vec_bone_.size());
		for (int i = 0; i < vec_bone_.size(); i++)
		{
			final_bone_transform_[i] = vec_bone_[i].transform * vec_bone_[i].offset;
		}
	}
	else
	{
		CalcBoneAnimTransform(anim2, normalized_time - trans_begin_norm_time, root_transform);
	}
}

void Skeleton::BlendBoneAnimTransform(const Animation& anim1, const Animation& anim2, float normalized_time, float weight, const mat4& root_transform) {
	// calculate hierarchy transform
	for (int i = 0; i < vec_bone_.size(); i++)
	{
		mat4 pos1 = glm::translate(mat4(1.0f), anim1.GetPosition(vec_bone_[i].name, normalized_time));
		mat4 rot1 = glm::mat4_cast(anim1.GetRotation(vec_bone_[i].name, normalized_time));

		mat4 pos2 = glm::translate(mat4(1.0f), anim2.GetPosition(vec_bone_[i].name, normalized_time));
		mat4 rot2 = glm::mat4_cast(anim2.GetRotation(vec_bone_[i].name, normalized_time));

		pos1 = Interpolate(pos1, pos2, weight);
		rot1 = Interpolate(rot1, rot2, weight);

		int parent_i = vec_bone_[i].parent_index;
		// check if this bone have parent
		mat4 parent_mat = parent_i >= 0 ? vec_bone_[parent_i].transform : root_transform;
		vec_bone_[i].transform = parent_mat * pos1 * rot1;
	}

	// calculate final transform
	final_bone_transform_.resize(vec_bone_.size());
	for (int i = 0; i < vec_bone_.size(); i++)
	{
		final_bone_transform_[i] = vec_bone_[i].transform * vec_bone_[i].offset;
	}
}



void Skeleton::SetVertexBoneInfo(Vertex& vertex, unsigned int bone_index, float weight) const {
	for (unsigned int i = 0; i < kMaxBonePerVertex; i++)
	{
		if (vertex.bone_id[i] == -1)
		{
			vertex.bone_id[i] = bone_index;
			vertex.weights[i] = weight;
			return;
		}
	}
}