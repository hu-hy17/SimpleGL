#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec4 aBoneWeight0;
layout (location = 4) in vec4 aBoneWeight1;
layout (location = 5) in vec4 aBoneWeight2;
layout (location = 6) in vec4 aBoneIdx0;
layout (location = 7) in vec4 aBoneIdx1;
layout (location = 8) in vec4 aBoneIdx2;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

const int MAX_BONES = 100;

uniform mat4 localToGlobal;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 bones[MAX_BONES];
uniform bool animate; 

void main()
{   
    if(animate)
    {
        vec4 bPos = aBoneWeight0[0] * bones[int(aBoneIdx0[0])] * vec4(aPos, 1.0);
        vec3 tmpNorm = aBoneWeight0[0] * mat3(bones[int(aBoneIdx0[0])]) * aNormal;
        for(int i = 1; i < 4; i++)
        {
            bPos += aBoneWeight0[i] * bones[int(aBoneIdx0[i])] * vec4(aPos, 1.0);
            tmpNorm += aBoneWeight0[i] * mat3(bones[int(aBoneIdx0[i])]) * aNormal;
        }

        for(int i = 0; i < 4; i++)
        {
            bPos += aBoneWeight1[i] * bones[int(aBoneIdx1[i])] * vec4(aPos, 1.0);
            tmpNorm += aBoneWeight1[i] * mat3(bones[int(aBoneIdx1[i])]) * aNormal;
        }

        for(int i = 0; i < 4; i++)
        {
            bPos += aBoneWeight2[i] * bones[int(aBoneIdx2[i])] * vec4(aPos, 1.0);
            tmpNorm += aBoneWeight2[i] * mat3(bones[int(aBoneIdx2[i])]) * aNormal;
        }

        gl_Position = projection * view * localToGlobal * bPos;
        FragPos = vec3(localToGlobal * bPos);
        Normal = mat3(localToGlobal) * normalize(tmpNorm);
    }
    else
    {
        gl_Position = projection * view * localToGlobal * vec4(aPos, 1.0);
        FragPos = vec3(localToGlobal * vec4(aPos, 1.0));
        Normal = mat3(localToGlobal) * aNormal;
    }

    TexCoord = vec2(aTex);
}