#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aWeights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int kMaxTotalBoneNums = 100;
uniform mat4 bones[kMaxTotalBoneNums];

void main()
{
    mat4 totalBoneTransform = mat4(0.0f);
    vec4 totalPosition = vec4(0.0f);

    for(int i = 0 ; i < 4 ; i++)
    {
        if(aBoneIDs[i] == -1)
            continue;

        totalBoneTransform += bones[aBoneIDs[i]] * aWeights[i];
    }

    totalPosition = totalBoneTransform *  vec4(aPos,1.0f);

    FragPos = vec3(model * totalPosition);
    Normal = (transpose(inverse(model)) * totalBoneTransform * vec4(aNormal, 0.0)).xyz;
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * totalPosition;
}