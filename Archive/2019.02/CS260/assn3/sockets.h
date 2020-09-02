/******************************************************************************
*   Name : Cody Morgan
*   Class: CS260
*   Assn : 3
*   Date : 11 NOV 2019
*   Brief: hold all the linux babbles
*
******************************************************************************/

#pragma once
#ifndef SOCKETS_H
#define SOCKETS_H


#ifdef _WIN32
  #pragma comment(lib, "Ws2_32.lib")
  #include <WinSock2.h>
  #include <WS2tcpip.h>
#else
  #include <sys/socket.h>
  #include <netdb.h>
  #include <netinet/in.h>
  #include <netinet/ip.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <sys/ioctl.h>
  #include <string.h>
  #include <stdio.h>
  #include <string.h>
#endif

#ifdef WIN32
  #define SOCKET_TYPE SOCKET
  #define INVALID_SOCKET  (SOCKET)(~0)
  #define FIONBIO     _IOW('f', 126, u_long) /* set/clear non-blocking i/o */
#else
  #define SOCKET_TYPE int
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define NO_ERROR 0
  #define InetPton inet_pton
#endif 

#endif
