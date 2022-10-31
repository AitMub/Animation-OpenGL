#include"animation.h"
#include"utility/anim_math.h"

Animation::Animation(const aiAnimation * anim) :
	anim_name_(anim->mName.data),
	total_frames_(anim->mDuration),
	frame_per_sec_(anim->mTicksPerSecond),
	total_sec_(anim->mDuration / anim->mTicksPerSecond)
{
	for (int i = 0; i < anim->mNumChannels; i++)
	{
		Channel channel;
		channel.name_ = anim->mChannels[i]->mNodeName.data;

		PositionKeyFrame pos_key;
		for (int j = 0; j < anim->mChannels[i]->mNumPositionKeys; j++)
		{
			pos_key.position = Convert<vec3>(anim->mChannels[i]->mPositionKeys[j].mValue);
			pos_key.time = anim->mChannels[i]->mPositionKeys[j].mTime;
			channel.position_channels_.emplace_back(pos_key);
		}

		RotationKeyFrame rot_key;
		for (int j = 0; j < anim->mChannels[i]->mNumRotationKeys; j++)
		{
			rot_key.quaternion = Convert<quat>(anim->mChannels[i]->mRotationKeys[j].mValue);
			rot_key.time = anim->mChannels[i]->mPositionKeys[j].mTime;
			channel.rotation_channels_.emplace_back(rot_key);
		}

		ScaleKeyFrame scale_key;
		for (int j = 0; j < anim->mChannels[i]->mNumScalingKeys; j++)
		{
			scale_key.scale = Convert<float>(anim->mChannels[i]->mScalingKeys[j].mValue);
			scale_key.time = anim->mChannels[i]->mPositionKeys[j].mTime;
			channel.scale_channels_.emplace_back(scale_key);
		}

		vec_channels_.emplace_back(channel);
		channel_name_to_index_[channel.name_] = i;
	}
}


float Animation::GetNormalizedTime(float time_in_seconds) const {
	time_in_seconds = fmod(time_in_seconds, total_sec_);
	return time_in_seconds / total_sec_;
}


glm::vec3 Animation::GetPosition(const string& channel_name, float time, bool time_normalized) const {
	float anim_time = GetAnimTime(time, time_normalized);

	unsigned int channel_index = channel_name_to_index_.find(channel_name)->second;
	const Channel& channel = vec_channels_[channel_index];
	
	if (channel.position_channels_.size() > 1)
	{
		unsigned int curr_key = FindKey(channel.position_channels_, anim_time);
		float t1 = channel.position_channels_[curr_key].time;
		float t2 = channel.position_channels_[curr_key + 1].time;
		float factor = (anim_time - t1) / (t2 - t1);
		return Interpolate(channel.position_channels_[curr_key].position,
			channel.position_channels_[curr_key + 1].position,
			factor);
	}
	else if (channel.position_channels_.size() == 1)
	{
		return channel.position_channels_[0].position;
	}
	else
	{
		return vec3(1.0f);
	}
}
glm::quat Animation::GetRotation(const string& channel_name, float time, bool time_normalized) const {
	float anim_time = GetAnimTime(time, time_normalized);

	unsigned int channel_index = channel_name_to_index_.find(channel_name)->second;
	const Channel& channel = vec_channels_[channel_index];

	if (channel.rotation_channels_.size() > 1)
	{
		unsigned int curr_key = FindKey(channel.rotation_channels_, anim_time);
		float t1 = channel.rotation_channels_[curr_key].time;
		float t2 = channel.rotation_channels_[curr_key + 1].time;
		float factor = (anim_time - t1) / (t2 - t1);
		return Interpolate(channel.rotation_channels_[curr_key].quaternion,
			channel.rotation_channels_[curr_key + 1].quaternion,
			factor);
	}
	else if (channel.rotation_channels_.size() == 1)
	{
		return channel.rotation_channels_[0].quaternion;
	}
	else
	{
		return quat();
	}
}
float Animation::GetScale(const string& channel_name, float time, bool time_normalized) const {
	float anim_time = GetAnimTime(time, time_normalized);

	unsigned int channel_index = channel_name_to_index_.find(channel_name)->second;
	const Channel& channel = vec_channels_[channel_index];

	if (channel.scale_channels_.size() > 1)
	{
		unsigned int curr_key = FindKey(channel.scale_channels_, anim_time);
		float t1 = channel.scale_channels_[curr_key].time;
		float t2 = channel.scale_channels_[curr_key + 1].time;
		float factor = (anim_time - t1) / (t2 - t1);
		return Interpolate(channel.scale_channels_[curr_key].scale,
			channel.scale_channels_[curr_key + 1].scale,
			factor);
	}
	else if (channel.scale_channels_.size() == 1)
	{
		return channel.scale_channels_[0].scale;
	}
	else
	{
		return 1.0f;
	}
}

inline float Animation::GetAnimTime(float time, bool time_normalized) const {
	float anim_time = time_normalized ? time * total_frames_ : time * frame_per_sec_;
	return fmod(anim_time, total_frames_);
}

int Animation::FindKey(const vector<PositionKeyFrame>& channel, float time) const {
	for (unsigned int i = 0; i < channel.size() - 1; i++)
	{
		if (time < channel[i + 1].time)
		{
			return i;
		}
	}
	return -1;
}
int Animation::FindKey(const vector<RotationKeyFrame>& channel, float time) const {
	for (unsigned int i = 0; i < channel.size() - 1; i++)
	{
		if (time < channel[i + 1].time)
		{
			return i;
		}
	}
	return -1;
}
int Animation::FindKey(const vector<ScaleKeyFrame>& channel, float time) const {
	for (unsigned int i = 0; i < channel.size() - 1; i++)
	{
		if (time < channel[i + 1].time)
		{
			return i;
		}
	}
	return -1;
}