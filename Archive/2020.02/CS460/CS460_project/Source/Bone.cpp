#include "Bone.h"
#include "Common.h"
  using std::vector;
  using std::cout;
#include "ObjectManager.h"
#include "Line.h"

Bone::Bone(std::string name) : name(name)
{
  offsetMatrix = glm::mat4x4(1.0f);
  transform = glm::mat4x4(1.0f);
  boneToModel = glm::mat4x4(1.0f);
  skinTransform = glm::mat4x4(1.0f);
  originalTransform_ = glm::mat4x4(1.0f);
  inverseK = glm::mat4x4(1.0f);
  children.reserve(20);
}

void Bone::reset()
{
  transform = originalTransform_;
}

bool Bone::isNamed(std::string isYourName)
{
  return name.find(isYourName) != -1;
}

void Bone::printBone(bool printChildren, bool withMat, std::string level)
{
  std::cout << level << ":::" << name << '\n';

  if (withMat)
  {
    printf("%sTransform\n", level.c_str());
    PrintMat(transform, level);

    printf("%sBone to World\n", level.c_str());
    PrintMat(boneToModel, level);

    printf("%sOffset\n", level.c_str());
    PrintMat(offsetMatrix, level);

    printf("%sSkin\n", level.c_str());
    PrintMat(skinTransform, level);
    cout << '\n';
  }

  if (printChildren)
  {
    for(auto child : children)
    {
      child.printBone(printChildren, withMat, level + "  ");
    }
  }
}

glm::vec3 Bone::getBonePosition() const
{
  return glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
}

auto Bone::getModelPosition() const -> glm::vec3
{
  return  glm::vec3(boneToModel[3][0], boneToModel[3][1], boneToModel[3][2]);
  //return glm::vec3(boneToModel * glm::vec4(getBonePosition(), 1));
}

void Bone::setBonePosition(const glm::vec3& pos)
{
  transform[3][0] = pos.x;
  transform[3][1] = pos.y;
  transform[3][2] = pos.z;
}


Skeleton::Skeleton(const aiScene* scene, std::vector<Vertex>& verts)
{
  loadBones(scene->mRootNode->mChildren[2]); // load the metal man only
  printSkeleton(false);
  loadBoneOffsets(scene, scene->mRootNode->mChildren[2], verts);
  for(auto vert : verts)
  {
    vert.validateWeights();
  }
  loadAnimations(scene);
  boneUBO = ObjectManager::getObjectManager()->generateGenericUBO(sizeof(Bone::skinTransform), bonesWithWeight_);
}

static int index = 0;
void Skeleton::update(const glm::mat4x4& modelToWorld, Bone* parent, int i)
{
  bool iAmGroot = false;
  if(!parent)
  {
    parent = &root_;
    if (parent->isNamed("INVALID"))
      return;

    index = 0;
    parent->boneToModel = parent->transform;
    if (printUpdate_)
    {
      printf("applying BASE transform to %s\n", parent->name.c_str());
      parent->printBone(false, true);
    }
    modelToWorld_ = modelToWorld;
    createBones(modelToWorld);
    iAmGroot=  true;
  }

  for (auto& child : parent->children)
  {
    child.boneToModel = parent->boneToModel * child.transform * child.inverseK;
    child.skinTransform = child.boneToModel * child.offsetMatrix;
    if (child.hasWeight())
    {
      ObjectManager::getObjectManager()->updateGenericUBO(boneUBO, index, (void*)(&child.skinTransform[0][0]));

      if(printUpdate_) 
      {
        printf("applying %s to %s skin# %i\n", parent->name.c_str(), child.name.c_str(), index);
        child.printBone(false,true);
      }
      index++;
    }
    else if (printUpdate_)
    {
      printf("bone %s has no weight - not sending skin\n", child.name.c_str());
    }

    update(modelToWorld, &child, index);
  }

  // turn off the update - so it only prints one update
  if(iAmGroot)
  {
    printUpdate_ = false;
  }
}

void Skeleton::printSkeleton(bool withMatricex)
{
  root_.printBone(true, withMatricex);
}

bool Skeleton::getBone(std::string name, Bone** outBone, Bone* current)
{
  if(!current)
  {
    current = &root_;
  }

  if(current->isNamed(name))
  {
    *outBone = current;
    return true;
  }

  for(int i = 0; i < current->children.size(); i++)
  {
    if(getBone(name, outBone, &current->children[i]))
      return true;
  }

  return false;
}

void Skeleton::loadBones(const aiNode* node, Bone* bone, Bone* current)
{
  if (!bone)
  {
    bone = &root_;
  }
  bone->name = node->mName.C_Str();
  bonenames_.push_back(bone->name);
  bone->transform = ConvertaiMatToglm(node->mTransformation);

  // Recurse onto this node's children
  current = bone;
  for (unsigned int i = 0; i < node->mNumChildren; ++i)
  {
    bone = current;
    bone->children.push_back(Bone());
    bone = &bone->children.back();
    loadBones(node->mChildren[i], bone, current);
  }
  bone = current;
}

void Skeleton::loadBoneOffsets(const aiScene* scene, const aiNode* node, vector<Vertex>& verts)
{
  cout << "\nAI BONE ORDER\n";

  auto bones = scene->mMeshes[2]->mBones;
  bonesWithWeight_ = scene->mMeshes[2]->mNumBones;

  for(auto b = 0; b < bonesWithWeight_; b++)
  {
    auto aiBone = bones[b];
    cout << aiBone->mName.C_Str() << '\n';

    // bone weights
    for(auto w = 0; w < aiBone->mNumWeights; w++)
    {
      auto weight = aiBone->mWeights[w];

      verts[weight.mVertexId].addBone(b, weight.mWeight);
    }

    // bone offsets
    Bone* bone;
    if(getBone(aiBone->mName.C_Str(), &bone))
    {
      bone->offsetMatrix = ConvertaiMatToglm(aiBone->mOffsetMatrix);
      bone->giveWeight();
    }
  }
}

void Skeleton::loadAnimations(const aiScene* scene)
{
  // get all the animations
  for(auto animIndex = 0; animIndex < scene->mNumAnimations; animIndex++)
  {
    aiAnimation* anim = scene->mAnimations[animIndex];

    // channels are the bones
    for(auto keyFrameIndex = 0; keyFrameIndex < anim->mNumChannels; keyFrameIndex++)
    {
      aiNodeAnim* chan = anim->mChannels[keyFrameIndex];
      vector<glm::vec3>  myV;
      vector<Quaternion> myQ;
      vector<glm::vec3>  myS;
      vector<float>      myT;
      
      // stuff all the key frames into vertors and ship to animation manager
      for(auto k = 0; k < chan->mNumPositionKeys; k++)
      {
        auto V = chan->mPositionKeys[k];
        auto Q = chan->mRotationKeys[k];
        auto S = chan->mScalingKeys[k];

        // every keyframe seems to have a VQS at that time
        if(V.mTime != Q.mTime || V.mTime != S.mTime)
        {
          printf("I am making an assumtion that all VQS have equal parts");
          throw;
        }

        myV.push_back(ConvertaiVecToGlm(V.mValue));
        myQ.push_back(Q.mValue);
        myS.push_back(ConvertaiVecToGlm(S.mValue));
        myT.push_back(V.mTime);
      }
      
      animation.addAnimation(anim->mName.C_Str(), 
      chan->mNodeName.C_Str(),
      myV,
      myQ,
      myS,
      myT,
      anim->mDuration, 
      anim->mTicksPerSecond);
    }
  }
}

void Skeleton::updateBonesAfterAnimation()
{
  if(animation.getMode() == AnimationManager::PlayMode::Stopped)
    return;

  int i =0;
  for(auto name : bonenames_)
  {
    Bone* bone;
    getBone(name, &bone);

    if(animation.boneTransforms.find(name) != animation.boneTransforms.end())
      bone->transform = animation.boneTransforms[name];
  }
}

void Skeleton::createBones(glm::mat4x4 modelToWorld)
{
  modelToWorld_ = modelToWorld;
  for (auto name : bonenames_)
  {
    auto objectMgr = ObjectManager::getObjectManager();

    Bone* parentBone;
    getBone(name, &parentBone);

    for(int i = 0; i < parentBone->children.size(); i++)
    {
      Bone* childbone;
      getBone(parentBone->children[i].name, &childbone);

      std::string lineName = name + "-" + childbone->name + " Line";
      Object* lineObj = objectMgr->getFirstObjectByName(lineName);
      if(!lineObj)
      {
        lineObj = objectMgr->addLine(lineName);
        lineObj->material.diffuse = glm::vec3(1, 0, 0);
        lineObj->material.ambient = glm::vec3(1, 0, 0);
      }

      Line* line = dynamic_cast<Line*>(lineObj);
      line->setStart(modelToWorld_ * parentBone->boneToModel);
      line->setEnd(childbone->transform);
    }
  }
}

void Skeleton::resetIK()
{
  for(auto name : bonenames_)
  {
    Bone* bone = nullptr;
    getBone(name,&bone);
    bone->inverseK = glm::mat4x4(1.0);
  }
}

void Skeleton::runIK(glm::vec3 goal, float percentComplete, int bail2)
{
  // goal is in model space
  goal = glm::vec3(glm::inverse(modelToWorld_) * glm::vec4(goal, 1));

  // find end effector (ee)
  auto endEffector = findEndEffector();
  auto E = endEffector->getModelPosition(); // end effector world position

  // calculate G`
  auto Gprime = ((1 - percentComplete) * E) + (percentComplete * goal);
  //ObjectManager::getObjectManager()->getFirstObjectByName("gPrime")->setPosition(glm::vec3(modelToWorld_ * glm::vec4(Gprime, 1)));  ////DEBUG////
  //ObjectManager::getObjectManager()->getFirstObjectByName("E")->setPosition(glm::vec3(modelToWorld_ * glm::vec4(E, 1)));  ////white DEBUG////

  // loop until delta < e or ee position - G` < e
  float ep = 10.0f;
  float delta = 2 * ep;
  int bail = 3;
  while(delta > ep && bail-- > 0)
  {
    // loop every bone in chain
    auto N = endEffector->parent;
    while(N && bail2-- > 0)
    {
      // origin of rotation
      auto O = N->getModelPosition();
      //ObjectManager::getObjectManager()->getFirstObjectByName("O")->setPosition(glm::vec3(modelToWorld_ * glm::vec4(O, 1)));  ////yellow DEBUG////

      // current end effector (cee)
      auto Eprime = endEffector->getModelPosition();

      // ||B|| unit vector
      auto B = glm::normalize(Eprime - O);

      // ||C|| unit vector
      auto C = glm::normalize(Gprime - O);

      // angle of rotation
      float dot = glm::dot(B, C);
      if(glm::abs(dot) > 1)
        return;
      float a = glm::acos(dot);

      // axis of rotation
      auto A = glm::cross(B,C);
      auto Aprime = glm::vec3(glm::inverse(N->boneToModel) * glm::vec4(A,0));

      // accumulate rotation in node ik
      Quaternion q(Aprime, a);

      N->inverseK = N->inverseK * q.getRotationMat();

      update(modelToWorld_);

      N = N->parent;
    }

    auto newEnd = endEffector->getModelPosition();
    delta = glm::distance(newEnd, Gprime);
  }
}

void Skeleton::buildParents(Bone* bone)
{
  if(!bone)
    bone = &root_;

  for(auto& child : bone->children)
  {
    child.parent = bone;
    buildParents(&child);
  }
}

Bone* Skeleton::findEndEffector()
{
  Bone* endEffector = nullptr;
  getBone("lowerarm.L_end", &endEffector);

  return endEffector;
}

