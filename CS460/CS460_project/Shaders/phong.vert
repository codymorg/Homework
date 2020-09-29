#version 430 core
#define MAX_BONE_COUNT 32

layout (location = 0) in vec3   vPos;
layout (location = 1) in vec3   vNormal;
layout (location = 2) in ivec4  vBoneIndices;
layout (location = 3) in vec4   vBoneWeights;

uniform mat4 modelToWorld; // model
uniform mat4 worldToCam;   // view
uniform mat4 projection;   // projection

out vec3 normal;
out mat4 viewTrans;
out vec3 vertPosView;

// vertex shader phong lighting
layout (std140, binding = 1) uniform skinMats
{
  mat4x4 skinTransforms[MAX_BONE_COUNT];
};

void main()
{
  mat4 zero = {vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0)};
  mat4 id = {vec4(1,0,0,0),vec4(0,1,0,0),vec4(0,0,1,0),vec4(0,0,0,1)};
  mat4 viewModel = worldToCam * modelToWorld;
  mat4 skin = id;
  float sum = vBoneWeights.x + vBoneWeights.y + vBoneWeights.z + vBoneWeights.w;

  // last step - calculate the final skin mat
  if(abs(1 - sum) < 0.1)
  {
    skin = zero;
    if(vBoneIndices[0] >= 0)
      skin += vBoneWeights.x * skinTransforms[vBoneIndices[0]];

    if(vBoneIndices[1] >= 0)
      skin += vBoneWeights.y * skinTransforms[vBoneIndices[1]];

    if(vBoneIndices[2] >= 0)
      skin += vBoneWeights.z * skinTransforms[vBoneIndices[2]];
    
    if(vBoneIndices[3] >= 0)
      skin += vBoneWeights.w * skinTransforms[vBoneIndices[3]];
  }

  gl_Position = projection * viewModel * skin * vec4(vPos,1.0f);

  normal = normalize(mat3(transpose(inverse(viewModel))) * vNormal);

  vertPosView = (viewModel * vec4(vPos,1)).xyz;
  viewTrans = worldToCam;
}