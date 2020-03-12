#include <RoboCatClientPCH.h>
#include "SpriteComponent.h"

std::unique_ptr< RenderManager >	RenderManager::sInstance;

RenderManager::RenderManager()
{
	SDL_Rect viewport = GraphicsDriver::sInstance->GetLogicalViewport();

	// The view transform stores both the scale factor and offset for rendering textures
	mViewTransform.x = viewport.w / 2;
	mViewTransform.y = viewport.h / 2;
	mViewTransform.w = 100;
	mViewTransform.h = 100;
}


void RenderManager::StaticInit()
{
	sInstance.reset( new RenderManager() );
}


void RenderManager::AddComponent( SpriteComponent* inComponent )
{
	mComponents.push_back( inComponent );
}

void RenderManager::RemoveComponent( SpriteComponent* inComponent )
{
	int index = GetComponentIndex( inComponent );

	if( index != -1 )
	{
		int lastIndex = mComponents.size() - 1;
		if( index != lastIndex )
		{
			mComponents[ index ] = mComponents[ lastIndex ];
		}
		mComponents.pop_back();
	}
}

void RenderManager::AddLine(Vector2 start, Vector2 end, Vector3 color, float TTL, int ID)
{
  auto state = InputManager::sInstance->GetState();
  if (state.hyperYarnColor.mX == -1.0f && state.hyperYarnColor.mY != 1.0f)
  {
    InputManager::sInstance->GetState().hyperYarnColor.mY = 1.0f;
    InputManager::sInstance->GetState().validateHyperYarnOnServer = true;
    InputManager::sInstance->GetState().hyperYarnHit = 0;

    lines.push_back(Line(start, end, color, TTL, ID));
  }
}



int RenderManager::GetComponentIndex( SpriteComponent* inComponent ) const
{
	for( int i = 0, c = mComponents.size(); i < c; ++i )
	{
		if( mComponents[ i ] == inComponent )
		{
			return i;
		}
	}

	return -1;
}


//this part that renders the world is really a camera-
//in a more detailed engine, we'd have a list of cameras, and then render manager would
//render the cameras in order
void RenderManager::RenderComponents()
{
	//Get the logical viewport so we can pass this to the SpriteComponents when it's draw time
	SDL_Rect viewport = GraphicsDriver::sInstance->GetLogicalViewport();
	for( auto cIt = mComponents.begin(), end = mComponents.end(); cIt != end; ++cIt )
	{
		( *cIt )->Draw( mViewTransform );
	}
}

void RenderManager::RenderLines()
{
  for (int i = 0; i < lines.size(); i++)
  {
    Line& line = lines[i];
    if (line.isValid)
    {
      for (auto sprite : RenderManager::sInstance->mComponents)
      {
        // dont shoot ourselves
        if (sprite->mGameObject->GetNetworkId() != line.networkID)
        {

          // find where we are and how big the cat is
          Vector2 center = WorldToScreen(sprite->mGameObject->GetLocation());
          float objScale = sprite->mGameObject->GetScale();
          int width = static_cast<int>(sprite->mTexture->GetWidth() * objScale);
          int height = static_cast<int>(sprite->mTexture->GetHeight() * objScale);
          int radius = std::max(width, height);

          // let's just do circle line intersection since that's easy and CS350 has scarred me for life in reference to bounding volumes
          if (line.intersect(center, radius/2))
          {
            line.color = Vector3(255, 0, 0);
            InputManager::sInstance->GetState().hyperYarnHit = 1;
            std::cout << "hit! " << InputManager::sInstance->GetState().hyperYarnHit << "\n";
          }
          else
          {
            std::cout << "miss!" << InputManager::sInstance->GetState().hyperYarnHit << "\n";
          }
          if (InputManager::sInstance->GetState().hyperYarnColor.mX != -1.0f)
          {
            line.color = InputManager::sInstance->GetState().hyperYarnColor;
          }
        }
      }
      line.update();
      line.draw();

    }
    else
    {
      lines.erase(lines.begin() + i);
      InputManager::sInstance->GetState().hyperYarnColor = Vector3(-1.0f, -1.0f, -1.0f);

    }
  }
}

void RenderManager::Render()
{
	//
    // Clear the back buffer
    //
  SDL_SetRenderDrawColor(GraphicsDriver::sInstance->GetRenderer(), 100, 149, 237, SDL_ALPHA_OPAQUE);
	GraphicsDriver::sInstance->Clear();
	
  RenderManager::sInstance->RenderLines();
	RenderManager::sInstance->RenderComponents();

	HUD::sInstance->Render();
	
	//
    // Present our back buffer to our front buffer
    //
	GraphicsDriver::sInstance->Present();

}

Line::Line(Vector2 start, Vector2 end, Vector3 color, float TTL, int ID) 
  : start_(start), end_(end), color(color), TTL_(TTL), networkID(ID)
{

  beginTime_ = std::chrono::high_resolution_clock::now();
}

void Line::update()
{
  auto current = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> diff = current - beginTime_;
  if (diff.count() > TTL_)
  {
    isValid = false;
  }

}

void Line::draw()
{
  auto renderer = GraphicsDriver::sInstance->GetRenderer();
  SDL_SetRenderDrawColor(renderer, color.mX, color.mY, color.mZ, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(renderer, start_.mX, start_.mY, end_.mX, end_.mY);
}

bool Line::intersect(Vector2 center, float radius)
{
  float dx = start_.mX - end_.mX;
  float dy = start_.mY - end_.mY;
  float len = sqrt((dx * dx) + (dy * dy));
  float dot = (((center.mX - start_.mX) * (end_.mX - start_.mX)) + ((center.mY - start_.mY) * (end_.mY - start_.mY))) / (len * len);
  float x = start_.mX + (dot * (end_.mX - start_.mX));
  float y = start_.mY + (dot * (end_.mY - start_.mY));

  float d1x = start_.mX - x;
  float d1y = start_.mY - y;
  float d2x = end_.mX - x;
  float d2y = end_.mY - y;

  float dist1 = sqrt((d1x * d1x) + (d1y * d1y));
  float dist2 = sqrt((d2x * d2x) + (d2y * d2y));

  float distx = center.mX - x;
  float disty = center.mY - y;
  float dist = sqrt((distx * distx) + (disty * disty));

  return dist < radius && (dist1 + dist2) <= (len+0.01f);
}

