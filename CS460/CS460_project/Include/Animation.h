#pragma once
#include <string>
#include <vector>
#include <map>
#include <glm/mat4x4.hpp>
#include "Quaternion.h"

class AnimationManager
{
public:
  enum class PlayMode
  {
    Stopped,
    FrameByFrame,
    PlayOnce,
    Loop
  };

  struct Animation
  {
    struct KeyFrames
    {
      std::vector<glm::vec3>  V;                     // translation
      std::vector<Quaternion> Q;                     // rotation
      std::vector<glm::vec3>  S;                     // scale
      std::vector<float>      tickMarks;             // transition tick to next transform
      int                     currentKeyFrame = 0;   // index into VQS
    };

    std::map<std::string, KeyFrames> keyFrameMap;           // [bone name][key frames]
    float                            animationDuration = 0; // total time in ticks
    float                            ticksPerSec = 0;       // probably 24
  };

  AnimationManager() = default;

  void addAnimation(const std::string& animName, 
                    const std::string& boneName, 
                    const std::vector<glm::vec3>& v,
                    const std::vector<Quaternion>& q,
                    const std::vector<glm::vec3>& s, 
                    const std::vector<float>& ticks,
                    float durationInTicks, 
                    float ticksPerSec);
  void setActive(std::string animName, PlayMode mode = PlayMode::Loop);
  void update();

  PlayMode playmode = PlayMode::Loop;
  std::map<std::string, glm::mat4x4> boneTransforms;

private:
  std::string animationName_;
  float currentTick_ = 0;

  std::map<std::string, Animation> animationMap_; // [animation name][keyframe struct]

  void setKeyFrame();
  glm::vec3 lerp(const glm::vec3& start, const glm::vec3& end, float percent);
  glm::mat4x4 vecToScaleMat(const glm::vec3 scale);
  glm::mat4x4 vecToTranslationMat(const glm::vec3 trans);
};