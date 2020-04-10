#ifdef _WIN32
#include <SDL_events.h>
#endif

class Engine
{
public:
  std::string username_ = "INVALID_USERNAME";
  std::string password_ = "INVALID_PASSWORD";
  std::string avatar_ = "Avatar1.png";
  std::string session_ = "INVALID_SESSION";
  std::string token_ = "INVALID_TOKEN";
  std::string server_ = "INVALID_SERVER";
	virtual ~Engine();
	static std::unique_ptr< Engine >	sInstance;

	virtual int		Run();
	void			SetShouldKeepRunning( bool inShouldKeepRunning ) { mShouldKeepRunning = inShouldKeepRunning; }
#ifdef _WIN32
	virtual void	HandleEvent( SDL_Event* inEvent );
#endif

protected:

	Engine();

	virtual void	DoFrame();

private:


			
			int		DoRunLoop();

			bool	mShouldKeepRunning;



};