
#include "EventClass.h"
#include "assert.h"

EventClass::EventClass()
{}


EventClass::~EventClass()
{}

void EventClass::Initparameter(const char * parameterName)
{
  assert(desc->getParameter(parameterName, &paramDesc) == FMOD_OK);
  paramIdx = paramDesc.index;
  paramVal = 0.0;
}

void EventClass::Play(float p)
{

}
