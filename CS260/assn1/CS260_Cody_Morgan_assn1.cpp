/******************************************************************************
*   Name : Cody Morgan
*   Class: CS260
*   Assn : 1
*   Date : 23 SEP 2019
*   Brief: send a UDP pack to a known server
*
******************************************************************************/

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <iostream>
using std::cout;


enum ErrorCodes
{
    noerror,
    wsaStartupFailure,
    addressFailure,
    socketFailure,
    sendBufferFull,
    receiveFailure
};

int main(int argc, const char* argv[])
{
    // setup WSA
    WSADATA networkData;
    int error = 0;
    error = WSAStartup(MAKEWORD(2, 2), &networkData);
    if (error)
    {
        error = WSAGetLastError();
        cout << "WSA startup failed error code: " << error << "\n";
        return wsaStartupFailure;
    }

    // setup socket info
    sockaddr_in address;
    memset(address.sin_zero, 0, sizeof(address.sin_zero));
    address.sin_family = AF_INET;
    address.sin_port = htons(8888);
    address.sin_addr.S_un.S_un_b.s_b1 = 54;
    address.sin_addr.S_un.S_un_b.s_b2 = 190;
    address.sin_addr.S_un.S_un_b.s_b3 = 216;
    address.sin_addr.S_un.S_un_b.s_b4 = 85;

    SOCKET udpSocket = INVALID_SOCKET;
    udpSocket = socket(address.sin_family, SOCK_DGRAM, 0);
    if (udpSocket == INVALID_SOCKET)
    {
        cout << "socket failed: WSA error: " << WSAGetLastError();
        WSACleanup();

        return socketFailure;
    }

    error = InetPton(address.sin_family, "54.190.216.85", &address.sin_addr);
    if (error == INADDR_NONE)
    {
        cout << "inet failed: WSA error: " << WSAGetLastError();
        WSACleanup();

        return socketFailure;
    }


    int sentCount = sendto(udpSocket, argv[0], strlen(argv[0]), 0, (SOCKADDR*) &address, sizeof(address));
    if (sentCount < 0)
    {
        cout << "send buffer count: " << sentCount << "\n";
        cout << "wsa error: " << WSAGetLastError();
        closesocket(udpSocket);
        WSACleanup();

        return sendBufferFull;
    }

    char buffer[1400] = { 0 };
    int messageSize;
    messageSize = recvfrom(udpSocket, buffer, 1400, 0, nullptr, nullptr);
    if (messageSize < 0)
    {
        cout << "recieve error: " << WSAGetLastError();
        return receiveFailure;
    }


    // print response
    for (int i = 0; i < messageSize ; i++)
    {
        cout << buffer[i];
    }

    // close socket
    error = closesocket(udpSocket);
    if (error == SOCKET_ERROR) {
        cout << "socket failed to close error: " << error << "WSAerror: " << WSAGetLastError();
        WSACleanup();
        return socketFailure;
    }

    return noerror;
}