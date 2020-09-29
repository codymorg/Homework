#include "..\Include\Animation.h"
#include <iostream>
#include "Common.h"

void AnimationManager::addAnimation(const std::string& animName, 
                                    const std::string& boneName, 
                                    const std::vector<glm::vec3>& v, 
                                    const std::vector<Quaternion>& q,
                                    const std::vector<glm::vec3>& s, 
                                    const std::vector<float>& ticks, 
                                    float durationInTicks, 
                                    float ticksPerSec)
{
  animationMap_[animName].keyFrameMap[boneName].V = v;
  animationMap_[animName].keyFrameMap[boneName].Q = q;
  animationMap_[animName].keyFrameMap[boneName].S = s;
  animationMap_[animName].keyFrameMap[boneName].tickMarks = ticks;
  animationMap_[animName].animationDuration = durationInTicks;
  animationMap_[animName].ticksPerSec = ticksPerSec;

  //DEBUG
  if(animationName_.empty())
    animationName_ = "Armature|walk";
}

void AnimationManager::setActive(std::string animName, PlayMode mode)
{
}

void AnimationManager::update()
{
  // check to make sure key frame is current
  setKeyFrame();
  if(playmode == PlayMode::Stopped)
    return;

  // get current animation
  auto& currentAnimation = animationMap_[animationName_];
  for (auto& keyblock : currentAnimation.keyFrameMap)
  {
    int keyframe = keyblock.second.currentKeyFrame;

    // get start and end VQS for each bone
    auto startV = keyblock.second.V[keyframe];
    auto startQ = keyblock.second.Q[keyframe];
    auto startS = keyblock.second.S[keyframe];

    auto endV = keyblock.second.V[keyframe + 1];
    auto endQ = keyblock.second.Q[keyframe + 1];
    auto endS = keyblock.second.S[keyframe + 1];

    // get current percentage
    auto t = currentTick_ / keyblock.second.tickMarks[keyframe + 1];

    // lerp and slerp => data for this frame
    auto lerpV = vecToTranslationMat(lerp(startV, endV, t));
    auto slerped = startQ.slerp(endQ, t).getRotationMat();
    auto lerpS = vecToScaleMat(lerp(startS, endS, t));

    // set bone trans
    boneTransforms[keyblock.first] = lerpV * slerped * lerpS;
  }
}

void AnimationManager::setKeyFrame()
{
  if (playmode == PlayMode::Stopped)
    return;

  auto& currentAnimation = animationMap_[animationName_];
  bool endReached = false;

  // set frame to tick conversion
  currentTick_ += currentAnimation.ticksPerSec / 60.0f;

  // check tick hit times for every bone
  for(auto& keyblock : currentAnimation.keyFrameMap)
  {
    int& currentKeyFrame = keyblock.second.currentKeyFrame;

    // if current tick is equal to the next frame - deal with it
    if(currentTick_ >= keyblock.second.tickMarks[currentKeyFrame + 1])
    {
      // adjust for rounding error
      currentKeyFrame++;

      // if this is the last tick check the mode
      if(currentKeyFrame + 1 >= keyblock.second.tickMarks.size())
      {
        endReached =  true;
        switch (playmode)
        {
        case AnimationManager::PlayMode::PlayOnce:
          playmode = PlayMode::Stopped;
        case AnimationManager::PlayMode::Loop:
          currentKeyFrame = 0;
          break;
        default:
          break;
        }
      }
    }
  }
  
  if(endReached)
  {
    currentTick_ = 0;
  }
}

glm::vec3 AnimationManager::lerp(const glm::vec3& start, const glm::vec3& end, float t)
{
  return start + t * (end - start);
}

glm::mat4x4 AnimationManager::vecToTranslationMat(const glm::vec3 trans)
{
  auto scaleM = glm::mat4x4(1);
  scaleM[3][0] = trans.x;
  scaleM[3][1] = trans.y;
  scaleM[3][2] = trans.z;

  return scaleM;
}

glm::mat4x4 AnimationManager::vecToScaleMat(const glm::vec3 scale)
{
  auto transM = glm::mat4x4(1);
  transM[0][0] = scale.x;
  transM[1][1] = scale.y;
  transM[2][2] = scale.z;

  return transM;

}
