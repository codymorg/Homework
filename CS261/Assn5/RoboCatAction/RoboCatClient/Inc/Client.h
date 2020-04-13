#include <string>

class Client : public Engine
{
public:

	static bool StaticInit( );


	Client();

	virtual void	DoFrame() override;
	virtual void	HandleEvent( SDL_Event* inEvent ) override;

  std::string GetServer(const std::string& loginServer, 
                        const std::string& username, 
                        const std::string& password);
  



};