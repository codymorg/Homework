#include <string>

class Client : public Engine
{
public:

	static bool StaticInit( );

protected:

	Client();

	virtual void	DoFrame() override;
	virtual void	HandleEvent( SDL_Event* inEvent ) override;

  std::string GetServer(const std::string& loginServer, 
                        const std::string& username, 
                        const std::string& password);
  
private:
  std::string username_ = "INVALID_USERNAME";
  std::string password_ = "INVALID_PASSWORD";
  std::string avatar_ = "Avatar1.png";
  std::string session_ = "INVALID_SESSION";
  std::string token_ = "INVALID_TOKEN";
  std::string server_ = "INVALID_SERVER";


};