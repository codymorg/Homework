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

  auto getPosition() const -> glm::vec3;
  bool hasWeight() { return hasWeight_; };

  void setPosition(const glm::vec3& pos);
  void giveWeight(){hasWeight_ = true;};

  std::string name;
  std::vector<Bone> children;
  glm::mat4x4 offsetMatrix;
  glm::mat4x4 transform;
  glm::mat4x4 boneToModel;
  glm::mat4x4 skinTransform;

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

  AnimationManager animation;
  void updateBonesAfterAnimation();
private:

  Bone root_;
  int boneUBO = -1;
  int bonesWithWeight_ = 0;
  bool printUpdate_ = false;
  std::vector<std::string> bonenames_;
  glm::mat4x4 modelToWorld_;

  void loadBones(const aiNode* node, Bone* parent = nullptr, Bone* current = nullptr);
  void loadBoneOffsets(const aiScene* scene, const aiNode* node, std::vector<Vertex>& verts);
  void loadAnimations(const aiScene* scene);

};
