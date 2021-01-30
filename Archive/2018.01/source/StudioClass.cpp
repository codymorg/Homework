
#include "StudioClass.h"
#include <stdio.h>
#include <assert.h>


StudioClass::StudioClass() {
	Initialize();
}
void StudioClass::Initialize()
{
  // initialize the system
  FMOD::Studio::System::create(&studioSystem);
  //get the lowlevel system
  studioSystem->getLowLevelSystem(&lowSystem);
  lowSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0);
  studioSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, NULL);

  //TODO: make something that will read the GUIDs and load banks like that
  //load the banks
  assert(studioSystem->loadBankFile("../Heroic Engine/Assets/Audio/Desktop/Master Bank.bank",FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank)         == FMOD_OK);
  assert(studioSystem->loadBankFile("../Heroic Engine/Assets/Audio/Desktop/Master Bank.strings.bank",FMOD_STUDIO_LOAD_BANK_NORMAL, &stringBank) == FMOD_OK);
  assert(studioSystem->loadBankFile("../Heroic Engine/Assets/Audio/Desktop/SFX.bank",FMOD_STUDIO_LOAD_BANK_NORMAL, &sfxBank)                    == FMOD_OK);         
  assert(studioSystem->loadBankFile("../Heroic Engine/Assets/Audio/Desktop/Music.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &musicBank)               == FMOD_OK);

  //create event instances
  //resize the sfx vector
  events.resize(EVENT_NUM);
  assert(studioSystem->getEvent("event:/SFX/SFX_Explosions", &(events[SFX_EXPLOSION].desc)) == FMOD_OK);
  assert(studioSystem->getEvent("event:/SFX/SFX_FlyBy", &(events[SFX_FLYBY].desc)) == FMOD_OK);
  assert(studioSystem->getEvent("event:/SFX/Alarms/SFX_HealthCritical", &(events[SFX_HealthCritical].desc)) == FMOD_OK);
  assert(studioSystem->getEvent("event:/SFX/Energies/SFX_RayBlasts", &(events[SFX_RayBlasts].desc)) == FMOD_OK);
  assert(studioSystem->getEvent("event:/Music/Music_Level1", &(events[Music_Level1].desc)) == FMOD_OK);
  assert(studioSystem->getEvent("event:/Music/Music_StyleSample", &(events[MUSIC].desc)) == FMOD_OK);

  //create the parameter for the events
  assert(events[SFX_EXPLOSION].desc->getParameter("Explosions_Severity", &paramDesc) == FMOD_OK);
  explosionIdx = paramDesc.index;
  explosionParam = 0.0f;

  //move this to game specific file
  events[SFX_EXPLOSION].Initparameter("Explosions_Severity");
  events[SFX_FLYBY].Initparameter("FlyBy_SizeOfThing");
  events[SFX_RayBlasts].Initparameter("RayBlasts_SizeOfRayBlast");
  
  assert(events[SFX_FLYBY].desc->getParameter("FlyBy_SizeOfThing", &paramDesc) == FMOD_OK);
  explosionIdx = paramDesc.index;
  explosionParam = 0.0f;
}

void StudioClass::Update(float dt)
{
  studioSystem->update();
}

void StudioClass::Shutdown()
{
  //unload banks
  sfxBank->unload();
  stringBank->unload();
  masterBank->unload();

  //release system
  studioSystem->release();
}


void StudioClass::PlayEvent(EVENT_TAG eventTag, float param)
{

  FMOD::Studio::EventInstance* eventInst;
  events[eventTag].desc->createInstance(&eventInst);

  eventInst->setParameterValueByIndex(events[eventTag].GetParamIdx(), param);
  eventInst->start();
  eventInst->release();

}

void StudioClass::StopEvent(EVENT_TAG eventTag)
{
  //get the array of all instances of this event
  FMOD::Studio::EventInstance* instances[128];
  int count;
  events[eventTag].desc->getInstanceList(instances, 128, &count);

  //go through it and call stop on all of them
  for (int i = 0; i < count; ++i)
  {
    instances[i]->stop(FMOD_STUDIO_STOP_IMMEDIATE);
  }
}


StudioClass* StudioClass::studioClass = nullptr;

StudioClass* StudioClass::GetStudioClass() {
	
	if (studioClass)
		return studioClass;
	else
	{
		studioClass = new StudioClass();
		return studioClass;
	}
	return nullptr;
}



