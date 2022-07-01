#ifndef LIGHT_H
#define LIGHT_H

#include<glm/vec3.hpp>

using glm::vec3;

class Light
{
public:
	vec3 color_ = vec3(1.0f, 1.0f, 1.0f);
	vec3 pos_ = vec3(3.0f, 0.0f, 0.0f);
};

#endif