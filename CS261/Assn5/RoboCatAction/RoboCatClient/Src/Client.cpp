
#include <RoboCatClientPCH.h>
#include <cpprest/filestream.h>
  using namespace concurrency::streams; // Asynchronous streams
#include <cpprest/http_client.h>
  using web::http::client::http_client;
  using web::http::http_request;
  using web::http::http_response;
  using web::http::methods;
#include <cpprest/json.h>
  typedef web::json::value jsonValue;
#include <cpprest/uri.h>
#include <cpprest/uri_builder.h>
  using web::http::uri_builder;

// printing debug info
#include <chrono>
#include <ctime> 
#include <iostream>
  using std::cout;
  using std::wcout;

// converting narrow -> wide characters
#include <codecvt>
#include <locale>

// idc about your security errors windows
#ifdef _WIN32
#pragma warning(disable:4996)
#endif


bool Client::StaticInit( )
{
	// Create the Client pointer first because it initializes SDL
	Client* client = new Client();

	if( WindowManager::StaticInit() == false )
	{
		return false;
	}
	
	if( GraphicsDriver::StaticInit( WindowManager::sInstance->GetMainWindow() ) == false )
	{
		return false;
	}

	TextureManager::StaticInit();
	RenderManager::StaticInit();
	InputManager::StaticInit();

	HUD::StaticInit();

	sInstance.reset( client );

	return true;
}

string GetTime()
{
  std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  string time = std::ctime(&end_time);
  time = time.substr(time.find(' ')+1);
  time = time.substr(time.find(' ')+1);
  time = time.substr(time.find(' ')+1);
  time = time.substr(0,time.find(' '));
  return time;
}

void PrintResponse(http_response& response)
{
  wcout << response.status_code() << " " << response.reason_phrase().c_str();
  cout <<  " at " << GetTime() << "\n";
  stringstreambuf buffer;
  response.body().read_to_end(buffer).get();
  string body = buffer.collection();
  cout << " :::BEGIN:::\n" << "\t";
  for (size_t i = 0; i < body.size(); i++)
  {
    if (body[i] == '}')
      cout << '\n';
    cout << body[i];
    if (body[i] == '{' || body[i] == ',')
      cout << "\n\t";
    if (body[i] == ':' && body[i+1] =='"')
      cout << "\t";
  } 
  cout << "\n :::END:::\n\n";

  response.set_body(body);
}

string GetValueFromKey(http_response& response, const string& key)
{
  string value = key + " not found";
  stringstreambuf buffer;
  response.body().read_to_end(buffer).get();
  string body = buffer.collection();
  int index = body.find(key);
  if (index != string::npos)
  {
    value = body.substr(index + key.size() + 3);
    value = value.substr(0, value.find('"'));
  }
  response.set_body(body);
  return value;
}

string Client::GetServer(const string& loginServer, const string& username, const string& password)
{
  username_ = username;
  password_ = password;

  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring wideLogin = converter.from_bytes(loginServer);
  http_client httpClient(wideLogin);

  jsonValue json;
  json[L"username"] = jsonValue::string(utility::conversions::to_utf16string(username));
  json[L"password"] = jsonValue::string(utility::conversions::to_utf16string(password));

  
  // login user
  cout << "sending POST to login user at " << GetTime() << "\n";
  auto req = httpClient.request(methods::POST, L"/api/v1/login", json).then([&](http_response res)
  {
    cout << "POST response: ";
    PrintResponse(res);
    session_ = GetValueFromKey(res, "session");
    token_ = GetValueFromKey(res, "token");
    json[L"session"] = jsonValue::string(utility::conversions::to_utf16string(session_));
    json[L"token"] = jsonValue::string(utility::conversions::to_utf16string(token_));

      
  });
  req.wait();

  // getting game server
  cout << "Sending POST for server login at " << GetTime() << "\n";
  req = httpClient.request(methods::POST, L"/api/v1/connect", json).then([&](http_response res)
  {
    cout << "POST response : ";
    PrintResponse(res);
    server_ = GetValueFromKey(res, "server");
    avatar_ = GetValueFromKey(res, "avatar");
  });
  req.wait();

  cout << "Game Server Address: " + server_ << "\n";
  return server_;
}

Client::Client()
{
	GameObjectRegistry::sInstance->RegisterCreationFunction( 'RCAT', RoboCatClient::StaticCreate );
	GameObjectRegistry::sInstance->RegisterCreationFunction( 'MOUS', MouseClient::StaticCreate );
	GameObjectRegistry::sInstance->RegisterCreationFunction( 'YARN', YarnClient::StaticCreate );

	string loginServer = StringUtils::GetCommandLineArg( 1 );
	string name = StringUtils::GetCommandLineArg( 2 );
  string password = StringUtils::GetCommandLineArg(3);
  
  string destination = GetServer(loginServer, name, password);

	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString(destination);

	NetworkManagerClient::StaticInit( *serverAddress, name );

	//NetworkManagerClient::sInstance->SetDropPacketChance( 0.6f );
	//NetworkManagerClient::sInstance->SetSimulatedLatency( 0.25f );
	//NetworkManagerClient::sInstance->SetSimulatedLatency( 0.5f );
	//NetworkManagerClient::sInstance->SetSimulatedLatency( 0.1f );
}



void Client::DoFrame()
{
	InputManager::sInstance->Update();

	Engine::DoFrame();

	NetworkManagerClient::sInstance->ProcessIncomingPackets();

	RenderManager::sInstance->Render();

	NetworkManagerClient::sInstance->SendOutgoingPackets();
}

void Client::HandleEvent( SDL_Event* inEvent )
{
	switch( inEvent->type )
	{
	case SDL_KEYDOWN:
		InputManager::sInstance->HandleInput( EIA_Pressed, inEvent->key.keysym.sym );
		break;
	case SDL_KEYUP:
		InputManager::sInstance->HandleInput( EIA_Released, inEvent->key.keysym.sym );
		break;
	default:
		break;
	}
}

