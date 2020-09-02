#include <RoboCatServerPCH.h>

GrenadeServer::GrenadeServer()
{
	//yarn lives a second...
	mTimeToDie = Timing::sInstance.GetFrameStartTime() + 2.2f;
	timeToExplode = Timing::sInstance.GetFrameStartTime() + 2.0f;
}

void GrenadeServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject( this );
}


void GrenadeServer::Update()
{
  Grenade::Update();


  if( Timing::sInstance.GetFrameStartTime() > mTimeToDie )
	{
		SetDoesWantToDie( true );
	}


}

bool GrenadeServer::HandleCollisionWithCat( RoboCat* inCat )
{
	if(Timing::sInstance.GetFrameStartTime() > timeToExplode)
	{
		//kill yourself!
		SetDoesWantToDie( true );

		static_cast< RoboCatServer* >( inCat )->TakeDamage( GetPlayerId() );

	}

	return false;
}


