#ifndef RENDER_VOLUME_H
#define RENDER_VOLUME_H

struct RenderVolume
{
	float fov_in_degree;
	int screen_width;
	int screen_height;
	float near_z;
	float far_z;

	RenderVolume(float _fov_in_degree, int _screen_width, int _screen_height, float _near_z, float _far_z)
		: fov_in_degree(_fov_in_degree), screen_width(_screen_width), screen_height(_screen_height), near_z(_near_z), far_z(_far_z) {}
};

#endif