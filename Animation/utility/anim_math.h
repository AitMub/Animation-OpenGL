#ifndef  ANIM_MATH_H
#define ANIM_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <assimp/Importer.hpp>
using glm::mat4;
using glm::quat;
using glm::vec3;

// Converter
template<typename T1, typename T2>
T1 Convert(const T2& src);

template<>
mat4 Convert(const aiMatrix4x4& ai_mat4);

template<>
quat Convert(const aiQuaternion& ai_quat);

template<>
mat4 Convert(const aiQuaternion& ai_quat);

template<>
vec3 Convert(const aiVector3D& ai_vec);

template<>
float Convert(const aiVector3D& ai_vec);


// Interpolate
template<typename T>
T Interpolate(const T& value1, const T& value2, float factor) {
    return (value2 - value1) * factor + value1;
}

template<>
quat Interpolate<quat>(const quat& value1, const quat& value2, float factor);

#endif