#ifndef ANIMATION_H
#define ANIMATION_H

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include<string>
#include<vector>
#include<unordered_map>
using std::vector;
using std::string;
using std::unordered_map;


struct PositionKeyFrame
{
	glm::vec3 position;
	float time;
};

struct RotationKeyFrame
{
	glm::quat quaternion;
	float time;
};

struct ScaleKeyFrame
{
	float scale; // uniform
	float time;
};

struct Channel
{
	string name_;
	vector<PositionKeyFrame> position_channels_;
	vector<RotationKeyFrame> rotation_channels_;
	vector<ScaleKeyFrame> scale_channels_;
};

struct AnimState
{
	enum EState {eSoloPlaying, eBlending, eTransitioning};
	bool looping;
};

class Animation
{
public:
	Animation(const aiAnimation* anim);

	const string anim_name_;
	const int total_frames_;
	const float frame_per_sec_;

	glm::vec3 GetPosition(const string& channel_name, float time, bool time_normalized = false) const;
	glm::quat GetRotation(const string& channel_name, float time, bool time_normalized = false) const;
	float GetScale(const string& channel_name, float time, bool time_normalized = false) const;
private:
	vector<Channel> vec_channels_;
	unordered_map<string, unsigned int> channel_name_to_index_;

	int FindKey(const vector<PositionKeyFrame>& pos_channel, float time) const;
	int FindKey(const vector<RotationKeyFrame>& rot_channel, float time) const;
	int FindKey(const vector<ScaleKeyFrame>& scale_channel, float time) const;

	inline float GetAnimTime(float time, bool time_normalized) const;
};

#endif