#ifndef ANIMATION_H
#define ANIMATION_H

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include<string>
#include<vector>
using std::vector;

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

class Animation
{
public:
	void LoadAnimation();
	void ClearAnimation();

private:
	std::string anim_name_;
	int total_frames_;
	float frame_per_sec_;

	vector<vector<PositionKeyFrame>> position_channels_;
	vector<vector<RotationKeyFrame>> rotation_channels_;
	vector<vector<ScaleKeyFrame>> scale_channels_;
};

#endif