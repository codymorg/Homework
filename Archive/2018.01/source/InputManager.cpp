#include "InputManager.h"
#include "Physics.h"
#include "Transform.h"
#include "Entity.h"
#include "DirectXMath.h"


InputManager* InputManager::inputManager = nullptr;

InputManager* InputManager::GetInputManager()
{
  if(inputManager)
    return inputManager;
  else
  {
    inputManager = new InputManager();
    return inputManager;
  }
}

void InputManager::Update(float dt)
{
  std::list<Input*>::iterator it;

  for(it = registedInputs.begin(); it != registedInputs.end(); it++)
  {
    (*it)->Update(dt);
    bool up    = (*it)->GetButtonPressed(BUTTON_UP);
    bool down  = (*it)->GetButtonPressed(BUTTON_DOWN);
    bool right = (*it)->GetButtonPressed(BUTTON_RIGHT);
    bool left  = (*it)->GetButtonPressed(BUTTON_LEFT);
	bool buttonA = (*it)->GetButtonPressed(BUTTON_A);
	bool buttonB = (*it)->GetButtonPressed(BUTTON_B);
	bool buttonX = (*it)->GetButtonPressed(BUTTON_X);

    if(up)
      (*it)->parent->GetComponent<Physics>()->SetVelocity(0, 0.1f, 0);
    else if(right)
      (*it)->parent->GetComponent<Physics>()->SetVelocity(0.1f, 0, 0);
    else if(left)
      (*it)->parent->GetComponent<Physics>()->SetVelocity(-0.1f, 0, 0);
    else if(down)
      (*it)->parent->GetComponent<Physics>()->SetVelocity(0, -0.1f, 0);
    else if (!up &&!right&&!left&&!down)
      (*it)->parent->GetComponent<Physics>()->SetVelocity(0, 0, 0);

	if (buttonA) {
		(*it)->parent->GetComponent<Transform>()->rotation += 0.1f;
	}
	if (buttonB){ 
		Transform* transform = (*it)->parent->GetComponent<Transform>();
		transform->Setscale(transform->GetScaleX()*1.01f, transform->GetScaleY()*1.01f);
	}
	if (buttonX){
	}
  }
}

void InputManager::Add(Input* ent)
{
  registedInputs.push_back(ent);
}

void InputManager::Remove(VGUID ID)
{
 
}