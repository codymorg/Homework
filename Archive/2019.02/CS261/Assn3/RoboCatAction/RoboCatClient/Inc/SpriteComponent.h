#pragma once
#ifndef SpriteComponent__
#define SpriteComponent__

class SpriteComponent
{
public:

	SpriteComponent( GameObject* inGameObject );
	~SpriteComponent();

	virtual void		Draw( const SDL_Rect& inViewTransform );

			void		SetTexture( TexturePtr inTexture )			{ mTexture = inTexture; }

			Vector3		GetOrigin()					const			{ return mOrigin; }
			void		SetOrigin( const Vector3& inOrigin )		{ mOrigin = inOrigin; }

	GameObject*										mGameObject;
	TexturePtr										mTexture;

private:

	Vector3											mOrigin;


	//don't want circular reference...
};

typedef shared_ptr< SpriteComponent >	SpriteComponentPtr;

#endif