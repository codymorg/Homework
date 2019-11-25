/******************************************************************************
*   Name : Cody Morgan
*   Class: CS260
*   Assn : 3
*   Date : 11 NOV 2019
*   Brief: send a TCP packet to a known server
*
******************************************************************************/

#include "sockets.h"

#include <exception>
  using std::exception;
#include <iostream>
  using std::cout;
#include <string>
  using std::string;

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

  TCPconnection(const string& ip, unsigned port, const string& data = "")
  {
    CreateTCP();
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
      cout << ".";
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
    COUT << "[Connecting]\n";
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

    COUT << "\n[Connected]\n";
  };

  int SendData(const string& data)
  {
    int bytesToSend = int(data.length());
    const char* currentData = data.c_str();
    int sentBytes = SOCKET_ERROR;

    // keep sending until the OS takes it all
    COUT << "\n[Sending]\n...\n";
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

    int bytes = int(recv(sock_, buffer_, 5, 0));
    receiveBuffer += buffer_;
    memset(buffer_, 0, maxBytes_);
    
    // may return SOCET_ERROR
    return bytes;
  };

  void EndConnection()
  {
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

int main(int argc, char* argv[])
{
  bool initOK = Init();
  if (!initOK || argc > 2)
    return -1;

  try
  {
    string arg(argv[0]);
    TCPconnection tcp("54.190.216.85", 8888, arg);

    while (tcp.Update());

    cout << "\n" << tcp.receiveBuffer << "\n";
  }
  catch (TCPconnection::UnrecoverableError exception)
  {
    COUT << "UNRECOVERABLE ERROR\n\t";
    COUT << exception.what() << "\n\n";
  }
}