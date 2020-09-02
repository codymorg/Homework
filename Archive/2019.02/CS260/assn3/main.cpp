/******************************************************************************
*   Name : Cody Morgan
*   Class: CS260
*   Assn : 3
*   Date : 11 NOV 2019
*   Brief: send a TCP packet to a known server
*
******************************************************************************/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "sockets.h"

#include <exception>
  using std::exception;
#include <iostream>
  using std::cout;
#include <string>
  using std::string;

#ifndef _WIN32
  //#define _DEBUG
#endif

#ifdef _DEBUG
#define COUT if(true) cout
#else
#define COUT if(false) cout
#endif

void SleepWrapper(unsigned ms)
{
#ifdef _WIN32
  Sleep(ms);
#else
  usleep(ms * 1000);
#endif
}

// init sockets for windows
bool Init()
{

#ifdef WIN32
  // setup WSA
  WSADATA networkData;
  int error = WSAStartup(MAKEWORD(2, 2), &networkData);
  if (error)
  {
    error = WSAGetLastError();
    COUT << "WSA startup failed error code: " << error << "\n";

    return false;
  }
#endif

  // linux is always ok
  return true;
}

class TCPconnection
{
public:

  class UnrecoverableError : public exception
  {
  public:
    UnrecoverableError(string errorMsg) : errorMessage(errorMsg)
    {}

    virtual const char* what() const throw()
    {
      return errorMessage.c_str();
    }

    string errorMessage;
  };

  TCPconnection(const string& hostName, unsigned port, const string& data = "")
  {
    CreateTCP();

    // convert name to ip via dna
    hostent* host = gethostbyname(hostName.c_str());
    if (!host)
      throw UnrecoverableError("No Such Host " + hostName);
    char* ip = inet_ntoa(*((struct in_addr*)host->h_addr_list[0]));
    
    ConnectRemote(ip, port);
    
    if (!data.empty())
      SendData(data);
  }


  void CreateTCP()
  {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ == INVALID_SOCKET)
      throw UnrecoverableError("Failed to create TCP");
    else
    {
      #ifdef _WIN32
        int error = ioctlsocket(sock_, FIONBIO, &blockingMode);
      #else
        int error = ioctl(sock_, FIONBIO, &blockingMode);
      #endif

      if (error != NO_ERROR)
      {
#ifdef _WIN32
        closesocket(sock_);
#else
        close(sock_);
#endif
        sock_ = INVALID_SOCKET;
        throw UnrecoverableError(" failed to create non blocking ioctlsocket error:" + std::to_string(error));
      }
    }
  };
  
  bool Update()
  {
    if (ttl_ <= 0)
    {
      COUT << "[timeout]\n\n";
      return false;
    }
    ttl_ -= 100;

    int bytes = ReceiveData();
    if (bytes < 0) 
    {
      COUT << ".";
      fflush(0);
      SleepWrapper(100);
    }
    else if (bytes > 0)
    {
      COUT << "\n  [Data Recieved]\n" << this->receiveBuffer << "\n";
    }
    else
    {
      COUT << "\n  [Terminating Connection]\n";
      EndConnection();
      return false;
    }

    return true;
  }

  void ConnectRemote(const string& ip, unsigned port)
  {
    // set up address
    sockAddress_ = static_cast<sockaddr_in*>(calloc(sizeof(*sockAddress_), 1));
    if (!sockAddress_)
      throw std::bad_alloc();

    sockAddress_->sin_family = AF_INET;

    port_ = int(htons(port));
    sockAddress_->sin_port = (unsigned short)(port_);

    unsigned error = InetPton(sockAddress_->sin_family, ip.c_str(), &sockAddress_->sin_addr);
    if (error == INADDR_NONE)
    {
      #ifdef _WIN32
        COUT << "inet failed: WSA error: " << WSAGetLastError();
        WSACleanup();
      #else   
        COUT << "inet failed\n";
      #endif

      delete sockAddress_;

#ifdef _WIN32
      closesocket(sock_);
#else
      close(sock_);
#endif

      sock_ = INVALID_SOCKET;
      throw UnrecoverableError("invalid address inetPton error: " + std::to_string(error));
    }

    //connect to address
    COUT << "[Connecting to " << ip <<"]\n";
    #ifdef _WIN32
      do
      {
        connect(sock_, (sockaddr*)(sockAddress_), sizeof(sockaddr_in));
        COUT << "* ";
      } while (WSAGetLastError() == WSAEWOULDBLOCK);
    #else
      do
      {
        connect(sock_, (sockaddr*)(sockAddress_), sizeof(sockaddr_in));
        COUT << "* ";
      } while (errno == EAGAIN || errno == EWOULDBLOCK);
    #endif

    COUT << "\n[Sucessfully connected to " << ip <<"]\n";
  };

  int SendData(const string& data)
  {
    int bytesToSend = int(data.length());
    const char* currentData = data.c_str();
    int sentBytes = SOCKET_ERROR;

    // keep sending until the OS takes it all
    COUT << "\n[Sending]\n...\n";
    COUT << ">>\n";
    COUT << data << "\n<<\n";

    while (bytesToSend)
    {
      sentBytes = int(send(sock_, currentData, bytesToSend, 0));

      // hit an error - return now
      if (sentBytes == SOCKET_ERROR)
      {
        continue;
      }

      // move our eye to the right , but dont send over our buffer size
      currentData += sentBytes;
      bytesToSend -= sentBytes;
    }

    COUT << "[Complete] \n";

    // may return SOCKET_ERROR
    return sentBytes;
  };

  int ReceiveData()
  {
    if (!buffer_)
      buffer_ = (char*)calloc(maxBytes_, 1);

    int bytes = int(recv(sock_, buffer_, maxBytes_, 0));
    receiveBuffer += buffer_;
    memset(buffer_, 0, maxBytes_);
    
    // may return SOCET_ERROR
    return bytes;
  };

  void EndConnection()
  {
    COUT << "[Ending Connection]\n";
#ifdef _WIN32
    closesocket(sock_);
#else
    close(sock_);
#endif
  };

  string receiveBuffer;
private:

  SOCKET_TYPE sock_         = INVALID_SOCKET;
  string ip_                = "127.0.0.1";
  int port_                 = -1;
  sockaddr_in* sockAddress_ = nullptr;
  u_long blockingMode       = 1;
  unsigned maxBytes_        = 1024;
  char* buffer_             = nullptr;// send buffer
  int ttl_                  = 5000;   // in ms
};

string ParseAddress(const char* address)
{
  volatile unsigned u = 0;
  string hostName;
  int slashCount = 0;

  // determine if there is "https:// before the address
  for (u; u < 8; u++)
  {
    if (address[u] == '\0')
      break;

    // there must be https://
    if (address[u] == ':')
    {
      slashCount+=2;
      u += 3; // "://"
      break;
    }
  }

  // no prefix found - start copying from the beginning
  if (slashCount == 0)
    u = 0;

  // copy all host name
  while (address[u] != '\0' && address[u] != '/')
  {
    hostName += address[u++];
  }

  return hostName;
}

void Print_HTTP_Page(const string& response)
{
  int size = int(response.size());
  int beginI = int(response.find("Content-Length: ") + strlen("Content-Length: "));
  int endI = int(response.find("\n", beginI));
  int messageLength = int(std::stoi(response.substr(beginI, endI)));
  if (size - messageLength > size || size - messageLength < 0)
    cout << "[Error : Invalid Message Length]";
  else
    cout << response.substr(size - messageLength);
}

int main(int argc, const char* argv[])
{
  bool initOK = Init();
  if (!initOK || argc < 2)
  {
    cout << "\n ERROR : No host name specified\n\n";
    return -1;
  }

  string hostName = ParseAddress(argv[1]);

  try
  {
    string data = 
      "GET / HTTP/1.1\r\n"
      "host: " + hostName + "\r\n"
      "\r\n";

    TCPconnection tcp(hostName, 80, data);

    COUT << "[Listening]\n";
    while (tcp.Update());

    //cout << "\n" << tcp.receiveBuffer << "\n";
    Print_HTTP_Page(tcp.receiveBuffer);
    tcp.EndConnection();
  }
  catch (TCPconnection::UnrecoverableError exception)
  {
    COUT << "UNRECOVERABLE ERROR\n\t";
    COUT << exception.what() << "\n\n";
  }
}