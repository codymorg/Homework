#include <iostream>
#include "DigestBuilder.h"

using namespace std;

int main()
{
    // In RoboCatServer, you will receive this (and the token) from the user in the Hello packet
    string user = "username";
    string avatar = "avatar";

    // In RoboCatAction, you should retreive the secret from the command line as the second argument (after the port)
    cout << "Enter the secret:" << endl;
    string secret;
    cin >> secret;

    // concatenate the values to build the message
    string message = user + avatar + secret;

    // In RoboCatServer, I used this in NetworkManagerServer::HandlePacketFromNewClient 
    DigestBuilder digestBuilder;  
    string digest = digestBuilder.BuildDigest(message);
    // In RoboCatAction... does the digest match the token provided by the client?

    cout << "Message was " << message << ", SHA-256-hashed, base64-encoded digest is " << digest << endl;

    return 0;
}