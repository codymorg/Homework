/*----------------------------------------------------------------------------*\
| FILE NAME: GUID.cpp                                                          |
|==============================================================================|
| CONTRIBUTER(S): Ryan Dugie                                                   |
| Copyright � 2018 DigiPen (USA) Corporation.                                  |
\*----------------------------------------------------------------------------*/

/*============================================================================*\
|| ------------------------------ INCLUDES ---------------------------------- ||
\*============================================================================*/

#include "VGUID.h"

/*============================================================================*\
|| --------------------------- GLOBAL VARIABLES ----------------------------- ||
\*============================================================================*/

int guidCur = 0;

/*============================================================================*\
|| -------------------------- Private FUNCTIONS ----------------------------- ||
\*============================================================================*/

/*============================================================================*\
|| -------------------------- GLOBAL FUNCTIONS ------------------------------ ||
\*============================================================================*/

/*----------------------------------------------------------------------------*\
| Function : createGUID                                                        |
|------------------------------------------------------------------------------|
| Description : creates a new GUID                                             |
| Inputs      : none                                                           |
| Outputs     : returns the new GUID                                           |
\*----------------------------------------------------------------------------*/
VGUID CreateVGUID()
{
	return guidCur++;
}