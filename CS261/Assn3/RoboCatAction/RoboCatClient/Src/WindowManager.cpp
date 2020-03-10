#include <RoboCatClientPCH.h>

std::unique_ptr< WindowManager >	WindowManager::sInstance;

bool WindowManager::StaticInit()
{
	SDL_Window* wnd = SDL_CreateWindow( "Robo Cat Action!", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	
	if (wnd == nullptr)
	{
		SDL_LogError( SDL_LOG_CATEGORY_ERROR, "Failed to create window." );
		return false;
	}

 	sInstance.reset( new WindowManager( wnd ) );

	return true;
}


WindowManager::WindowManager( SDL_Window* inMainWindow )
{
	mMainWindow = inMainWindow;	

}

WindowManager::~WindowManager()
{
	SDL_DestroyWindow( mMainWindow );
}
