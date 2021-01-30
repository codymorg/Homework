/******************************************************************************/
/*!
\file   Panels.cpp
\author Werner Chen
\par    email: werner.chen\@digipen.edu

\date   09/25/2018
\brief
		The file is where all the panles should be defined

*/
/******************************************************************************/

#include "DesignPanel.h"
#include "Panels.h"


void ExamplePanel(std::map<std::string, void*> vmap) {
	ImGui::Checkbox("No titlebar", (bool*)vmap["no_titlebar"]); ImGui::SameLine(150);
	ImGui::Checkbox("No menu", (bool*)vmap["no_menu"]);
	ImGui::Checkbox("No move", (bool*)vmap["no_move"]); ImGui::SameLine(150);

}
