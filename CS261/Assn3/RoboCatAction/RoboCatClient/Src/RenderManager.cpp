#include <RoboCatClientPCH.h>

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

void RenderManager::AddLine(Vector2 start, Vector2 end, Vector3 color, float TTL)
{
  lines.push_back(Line(start, end, color, TTL));
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
      line.update();
      line.draw();
    }
    else
    {
      lines.erase(lines.begin() + i);
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

Line::Line(Vector2 start, Vector2 end, Vector3 color, float TTL) 
  : start_(start), end_(end), color_(color), TTL_(TTL)
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
  SDL_SetRenderDrawColor(renderer, color_.mX, color_.mY, color_.mZ, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(renderer, start_.mX, start_.mY, end_.mX, end_.mY);
}

