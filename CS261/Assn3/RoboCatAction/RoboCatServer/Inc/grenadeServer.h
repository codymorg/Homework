#pragma once
#ifndef GRENSERV_H
#define GRENSERV_H

class GrenadeServer : public Grenade
{
public:
	static GameObjectPtr	StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn( new GrenadeServer() ); }
	void HandleDying() override;

	virtual bool		HandleCollisionWithCat( RoboCat* inCat ) override;

	virtual void Update() override;

protected:
	GrenadeServer();

private:
	float mTimeToDie;
  float timeToExplode;
};
#endif