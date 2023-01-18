#ifndef LIGHT_H
#define LIGHT_H

#include<glm/vec3.hpp>

using glm::vec3;

struct Light
{
public:
	vec3 color_ = vec3(1.0f, 1.0f, 1.0f);
	vec3 pos_ = vec3(150.0f, 5.0f, 150.0f);
};

#endif