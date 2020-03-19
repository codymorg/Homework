class GrenadeClient : public Grenade
{
public:
	static	GameObjectPtr	StaticCreate()		{ return GameObjectPtr( new GrenadeClient() ); }

	virtual void		Read( InputMemoryBitStream& inInputStream ) override;
	virtual bool		HandleCollisionWithCat( RoboCat* inCat ) override;

protected:
  GrenadeClient();

private:

	SpriteComponentPtr	mSpriteComponent;
};