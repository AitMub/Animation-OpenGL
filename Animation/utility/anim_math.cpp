#include"anim_math.h"

template<>
mat4 Convert(const aiMatrix4x4& ai_mat4) {
    mat4 mat = glm::mat4();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            mat[j][i] = ai_mat4[i][j];
        }
    }
    return mat;
}

template<>
quat Convert(const aiQuaternion& ai_quat) {
    return glm::quat(ai_quat.w, ai_quat.x, ai_quat.y, ai_quat.z);
}

template<>
mat4 Convert(const aiQuaternion& ai_quat) {
    return glm::mat4_cast(Convert<quat>(ai_quat));
}

template<>
vec3 Convert(const aiVector3D& ai_vec) {
    return vec3(ai_vec.x, ai_vec.y, ai_vec.z);
}

template<>
float Convert(const aiVector3D& ai_vec) {
    return ai_vec.x;
}


template< >
quat Interpolate<quat>(const quat& value1, const quat& value2, float factor) {
    return glm::slerp(value1, value2, factor);
}
