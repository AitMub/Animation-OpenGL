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

// if using polymorphism, we need use vector<KeyFrame*> to store all key frame,
// considering the amount of key frame, the additional memory to store pointers
// may be relatively large, so I did't choose to use polymorphism
struct KeyFrame
{
	float time;
};

struct PositionKeyFrame : KeyFrame
{
	glm::vec3 position;
};

struct RotationKeyFrame : KeyFrame
{
	glm::quat quaternion;
};

struct ScaleKeyFrame : KeyFrame
{
	float scale; // uniform
};

struct Channel
{
	string name_;
	vector<PositionKeyFrame> position_channels_;
	vector<RotationKeyFrame> rotation_channels_;
	vector<ScaleKeyFrame> scale_channels_;
};


class Animation
{
public:
	Animation(const aiAnimation* anim);

	const string anim_name_;
	const int total_frames_;
	const float frame_per_sec_;
	const float total_sec_;

	float GetNormalizedTime(float time_in_seconds) const;

	glm::vec3 GetPosition(const string& channel_name, float time, bool time_normalized = true) const;
	glm::quat GetRotation(const string& channel_name, float time, bool time_normalized = true) const;
	float GetScale(const string& channel_name, float time, bool time_normalized = true) const;
private:
	vector<Channel> vec_channels_;
	unordered_map<string, unsigned int> channel_name_to_index_;

	int FindKey(const vector<PositionKeyFrame>& pos_channel, float time) const;
	int FindKey(const vector<RotationKeyFrame>& rot_channel, float time) const;
	int FindKey(const vector<ScaleKeyFrame>& scale_channel, float time) const;

	inline float GetAnimTime(float time, bool time_normalized) const;
};

#endif