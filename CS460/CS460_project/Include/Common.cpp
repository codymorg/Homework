#include "Common.h"
  using std::cout;
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "ReadAssimp.h"

glm::mat4x4 ConvertaiMatToglm(const aiMatrix4x4& mat)
{
  glm::mat4x4 glmMat;
  for (auto r = 0; r < 4; r++)
  {
    const float* row = mat[r];
    for (auto c = 0; c < 4; c++)
    {
      glmMat[r][c] = row[c];
    }
  }
  glmMat = glm::transpose(glmMat);
  return glmMat;
}

glm::vec3 ConvertaiVecToGlm(const aiVector3D ai)
{
  glm::vec3 vec;
  
  vec.x = ai.x;
  vec.y = ai.y;
  vec.z = ai.z;

  return vec;
}


void PrintMat(const glm::mat4x4& mat, std::string indent)
{
  cout << indent << "----------------------------------------------------------------------------------------------------\n";
  for (auto r = 0; r < 4; r++)
  {
    cout << indent;
    for (auto c = 0; c < mat.length(); c++)
    {
      auto col = mat[c];
      printf("%24.20f  ", col[r]);

    }
    cout << "\n";
  }
  cout << indent << "----------------------------------------------------------------------------------------------------\n";
}
