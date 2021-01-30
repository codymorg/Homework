/******************************************************************************/
/*!
\file   Input.cpp
\author Werner Chen
\par    email: werner.chen\@digipen.edu

\date   09/18/2018
\brief
		Input class implementation , only instantiation and update should be 
		called from anywhere outside the class.

*/
/******************************************************************************/


#include "Input.h"
#include <iostream>
#include "InputManager.h"
#pragma comment(lib, "XInput9_1_0.lib")   // Library. If your compiler doesn't support this type of lib include change to the corresponding one


//Public Functions




/*Default contructor for 1 player input */
Input ::Input(){
	this->mi_player_id = 1;
	this->m_input_type =  M_INPUT_CONTROLLER;
	this->m_default_type = M_INPUT_CONTROLLER;
	for (int u = 0; u < XINPUT_BUTTON_COUNT; u++)
		this->me_key_event.mpressed_[u] = false;
	if (!ConnectLoop()) {
		m_input_type = M_INPUT_KEYBOARD;
	}
  InputManager* m = InputManager::GetInputManager();
  m->Add(this);
}


/*Destruct*/
Input ::~Input() {

}


bool Input::GetButtonPressed(INPUT_BUTTON btn_code) {
	return me_key_event.mpressed_[btn_code];
}
float Input::GetAnalogInput(INPUT_ANALOG analog_code) {
	switch (analog_code) {
	case VERTICAL_R:
		return m_presence.Gamepad.sThumbRY;
	case HORIZONTAL_R:
		return m_presence.Gamepad.sThumbRX;
	case VERTICAL_L:
		return m_presence.Gamepad.sThumbLY;
	case HORIZONTAL_L:
		return m_presence.Gamepad.sThumbLX;
	}
	return 0;
}

/*Common contructor for general Input*/
Input::Input(int id, INPUT_TYPE type) {
	this->mi_player_id = id;
	this->m_input_type = type;
	this->m_default_type = type;
	for (int u = 0; u < XINPUT_BUTTON_COUNT; u++)
		this->me_key_event.mpressed_[u] = false;
	if (this->m_default_type == M_INPUT_CONTROLLER)
	{
		if (!ConnectLoop()) {
			m_input_type = M_INPUT_KEYBOARD;
		}
	}
	InputManager* m = InputManager::GetInputManager();
	m->Add(this);
}

/*Update funciton to check game input*/
void Input::Update(float dt) {
	//for (int u = 0; u < XINPUT_BUTTON_COUNT; u++)
		//me_key_event.mpressed_[u] = false;
	if (m_input_type == M_INPUT_CONTROLLER) {
		this->m_last = this->m_presence;
		DWORD dwResult = XInputGetState(0, &this->m_presence);
		if (dwResult != ERROR_SUCCESS){
     // std::cout << "Controller Disconnected, Please Reconnect\n";
			if (!ConnectLoop()) {
				m_input_type = M_INPUT_KEYBOARD;
				return;
			}
		}
		else  if (!CheckInputState()){
			// Call Input Handling Function Here
			XINPUT_GAMEPAD cgp = this->m_presence.Gamepad;
			XINPUT_GAMEPAD pgp = this->m_last.Gamepad;
			/*if (cgp.wButtons != pgp.wButtons)
				if (cgp.wButtons != 0)*/
			for (int u = 0; u < XINPUT_BUTTON_COUNT-2; u++)
				me_key_event.mpressed_[u] = (cgp.wButtons & (0x1 << u));
			if (TriggerThreshold(cgp.bLeftTrigger) != TriggerThreshold(pgp.bLeftTrigger))
				if (TriggerThreshold(cgp.bLeftTrigger)) me_key_event.mpressed_[BUTTON_LTRIGGER] = true;
			if (TriggerThreshold(cgp.bRightTrigger) != TriggerThreshold(pgp.bRightTrigger))
				if (TriggerThreshold(cgp.bRightTrigger)) me_key_event.mpressed_[BUTTON_RTRIGGER] = true;
			PrintInputState();
		}

	}
	else if (m_input_type == M_INPUT_KEYBOARD)
	{
		this->m_last = this->m_presence;
		DWORD dwResult = XInputGetState(0, &this->m_presence);
		if (dwResult == ERROR_SUCCESS){ 
			//std::cout << "Controller Dectected, Change to Controller Mode\n";
			m_input_type = M_INPUT_CONTROLLER;
		}
	}
	
}



// Private Functions
/*Check controller connection status*/
bool Input::IsXBOXControllerConnected()
{
	DWORD dwResult = XInputGetState(0,& (this->m_presence));
	if (dwResult == ERROR_SUCCESS)
		return true;
	else
		return false;
}


/*Helper function to print the buttons state*/
void Input:: PrintInputState()
{
	
	if (me_key_event.mpressed_[0])	std::cout << "UP ";
	if (me_key_event.mpressed_[1])	std::cout << "DOWN ";
	if (me_key_event.mpressed_[2])	std::cout << "LEFT ";
	if (me_key_event.mpressed_[3])	std::cout << "RIGHT ";
	if (me_key_event.mpressed_[4])	std::cout << "START ";
	if (me_key_event.mpressed_[5])	std::cout << "BACK ";
	if (me_key_event.mpressed_[6])	std::cout << "LTHUMB ";
	if (me_key_event.mpressed_[7])	std::cout << "RTHUMB ";
	if (me_key_event.mpressed_[8])	std::cout << "LS ";
	if (me_key_event.mpressed_[9])	std::cout << "RS ";
	if (me_key_event.mpressed_[12])	std::cout << "A ";
	if (me_key_event.mpressed_[13])	std::cout << "B ";
	if (me_key_event.mpressed_[14])	std::cout << "X ";
	if (me_key_event.mpressed_[15])	std::cout << "Y ";
	if (me_key_event.mpressed_[16])	std::cout << "LTRIGGER ";
	if (me_key_event.mpressed_[17])	std::cout << "RTRIGGER ";
 /* std::cout << "pressed" << std::endl;
	if (TriggerThreshold(cgp.bLeftTrigger) != TriggerThreshold(pgp.bLeftTrigger))
		if (TriggerThreshold(cgp.bLeftTrigger)) std::cout << "LT Press" << std::endl;
	if (TriggerThreshold(cgp.bRightTrigger) != TriggerThreshold(pgp.bRightTrigger))
		if (TriggerThreshold(cgp.bRightTrigger)) std::cout << "RT Press" << std::endl;
		*/
}


/*Check the trigger value over the threshold*/
bool Input::TriggerThreshold(BYTE byte) {
	return ((int)byte > XINPUT_TRIGGER_THRESHOLD) ? true : false;
}

/*Update the input state to check if anything new*/
bool Input::CheckInputState()
{
	XINPUT_GAMEPAD cgp = m_presence.Gamepad;
	XINPUT_GAMEPAD pgp = m_last.Gamepad;
	if (cgp.wButtons == pgp.wButtons
		&& TriggerThreshold(cgp.bLeftTrigger) == TriggerThreshold(pgp.bLeftTrigger)
		&& TriggerThreshold(cgp.bRightTrigger) == TriggerThreshold(pgp.bRightTrigger))
		return true;

	return false;
}

/*Loop the connection waiting for given time*/
bool Input::ConnectLoop() {
	bool printed_flag = false;
	float connect_timer = (float)clock();
	while (true) {
		if (!printed_flag) {
			//std::cout << "Please Connect the Controller\n";
			printed_flag = true;
		}
		if (IsXBOXControllerConnected()) {
			//std::cout << "Controller Dectected\n";
			return true;
		}
		if (clock() - connect_timer > CONNECTION_WAIT_TIME) {
			//std::cout << "No Controller Connected , Change to Keyboard Mode\n";
			return false;
		}
	}
}


// Input function for Keyboard mode
void Input::KeyBoardInput()
{

}

Input* InputCreate(int id, INPUT_TYPE type)
{
	return new Input(id, type);
}