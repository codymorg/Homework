
#include "SoundSystem.h"

SoundSystem::SoundSystem()
{}

SoundSystem::~SoundSystem()
{}

void SoundSystem::Initialize()
{
  if(FMOD::System_Create(&pSystem))
  {
    // report error
    return;
  }

  int driverCount = 0;
  pSystem->getNumDrivers(&driverCount);

  if(driverCount == 0)
  {
    // report error
    return;
  }

  // initialize instance with 36 channels
  pSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL, NULL);
}

void SoundSystem::Shutdown()
{
  // release all sounds in the container
  for(int i = 0; i < soundContainer.size(); ++i)
  {
    soundContainer[i]->release();
  }

  // release system
  pSystem->release();
}

void SoundSystem::Update(float dt)
{
}



int SoundSystem::CreateSound(const char * pFile)
{
  //create a new sound
  SoundClass pSound;
  pSystem->createSound(pFile, FMOD_DEFAULT, 0, &pSound);

  //put it at the end of the container
  soundContainer.push_back(pSound);

  return static_cast<int>(soundContainer.size() - 1);
}

void SoundSystem::PlaySound(int soundIdx, bool isLooping)
{
  if (isLooping)
  {
    //set the sound to loop
    soundContainer[soundIdx]->setMode(FMOD_LOOP_NORMAL);
    soundContainer[soundIdx]->setLoopCount(-1);
  }
  else
  {
    //set the sound to play once
    soundContainer[soundIdx]->setMode(FMOD_LOOP_OFF);
  }

  //play the sound
  pSystem->playSound(soundContainer[soundIdx], 0, false, 0);
}

void SoundSystem::releaseSound(int soundIdx)
{
  soundContainer[soundIdx]->release();
}

void SoundSystem::stopAllLooping()
{
  //find how many sounds are playing
  int numChannels = 0;

  //get the master channel group
  FMOD::ChannelGroup* masterChannelGroup;
  pSystem->getMasterChannelGroup(&masterChannelGroup);

  //go through eaacah channel and turn looping off
  masterChannelGroup->getNumChannels(&numChannels);
  for (int i = 0; i < numChannels; ++i)
  {
    FMOD::Channel* channel;
    masterChannelGroup->getChannel(i, &channel);
    channel->setMode(FMOD_LOOP_OFF);
  }
}

void SoundSystem::stopLooping(int soundIdx)
{
  soundContainer[soundIdx]->setLoopCount(0);
  soundContainer[soundIdx]->setMode(FMOD_LOOP_OFF);
}
