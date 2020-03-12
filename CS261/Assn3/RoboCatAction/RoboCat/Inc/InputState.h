#include "RoboMath.h"
class InputState
{
public:

	InputState() :
	mDesiredRightAmount( 0 ),
	mDesiredLeftAmount( 0 ),
	mDesiredForwardAmount( 0 ),
	mDesiredBackAmount( 0 ),
	mIsShooting( false )
	{}

	float GetDesiredHorizontalDelta()	const { return mDesiredRightAmount - mDesiredLeftAmount; }
	float GetDesiredVerticalDelta()		const { return mDesiredForwardAmount - mDesiredBackAmount; }
	bool  IsShooting()					const { return mIsShooting; }

	bool Write( OutputMemoryBitStream& inOutputStream ) const;
	bool Read( InputMemoryBitStream& inInputStream );
  bool hyperYarn = false;
  bool validateHyperYarnOnServer = false;
  bool hyperYarnHit = false;
  Vector3 hyperYarnColor = Vector3(-1.0f,-1.0f,-1.0f);
private:
	friend class InputManager;

	float	mDesiredRightAmount, mDesiredLeftAmount;
	float	mDesiredForwardAmount, mDesiredBackAmount;
	bool	mIsShooting;
};