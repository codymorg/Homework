#pragma once
#include <glm/mat4x4.hpp>

#include <string>
#include <vector>
#include <assimp/scene.h>

#include "Animation.h"

class Bone
{
public:
  Bone(std::string name = "INVALID");

  void reset();
  bool isNamed(std::string isYourName);
  void printBone(bool printChildren = false, bool withMat = false, std::string level = "");

  // model coord
  auto getBonePosition() const -> glm::vec3; // relative to node parent
  auto getModelPosition() const -> glm::vec3;
  bool hasWeight() { return hasWeight_; };

  void setBonePosition(const glm::vec3& pos);
  void giveWeight() { hasWeight_ = true; };

  std::string name;
  std::vector<Bone> children;
  Bone* parent = nullptr;

  glm::mat4x4 offsetMatrix;  // converts from bone space to mesh
  glm::mat4x4 transform;     // relative to node's parent         (N_L)
  glm::mat4x4 boneToModel;   // converts bone to root             (N_W)
  glm::mat4x4 skinTransform; // converts bone to model
  glm::mat4x4 inverseK;      // ik accumulation                   (N_K)

private:
  glm::mat4x4 originalTransform_;
  bool hasWeight_ = false;
};

class Skeleton
{
typedef class Vertex Vertex;
public:
  Skeleton(const aiScene* scene, std::vector<Vertex>& verts);
  Skeleton(){};

  void update(const glm::mat4x4& modelToWorld, Bone* parent = nullptr, int index = 0);
  
  void printUpdate(){ printUpdate_ = true; };
  void printSkeleton(bool withMatrices);

  bool getBone(std::string name, Bone** outBone, Bone* current = nullptr);
  auto getBoneNames()->std::vector<std::string> {return bonenames_;};
  void createBones(glm::mat4x4 modelToWorld);
  bool hasBones() { return !root_.isNamed("INVALID"); };

  Bone* findEndEffector();
  void resetIK();
  void runIK(glm::vec3 goal, float percentComplete, int bail2);
  void buildParents(Bone* bone = nullptr);

  AnimationManager animation;
  void updateBonesAfterAnimation();
private:

  Bone root_;
  int boneUBO = -1;
  int bonesWithWeight_ = 0;
  bool printUpdate_ = false;
  std::vector<std::string> bonenames_;
  glm::mat4x4 modelToWorld_;

  void loadBones(const aiNode* node, Bone* bone = nullptr, Bone* current = nullptr);
  void loadBoneOffsets(const aiScene* scene, const aiNode* node, std::vector<Vertex>& verts);
  void loadAnimations(const aiScene* scene);

};
