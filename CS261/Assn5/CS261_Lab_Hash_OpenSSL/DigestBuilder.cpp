//#include "RoboCatServerPCH.h"

//TODO: when including in RoboCat, uncomment the line above, then remove this line and the #include/using lines that follow
#include <string>           // Not needed in RoboCatAction
#include <cmath>            // Not needed in RoboCatAction
#include "DigestBuilder.h"  // Not needed in RoboCatAction
using namespace std;        // Not needed in RoboCatAction

// OpenSSL base64 encoding routines documented at https://www.openssl.org/docs/man1.1.0/man3/EVP_ENCODE_CTX_num.html
// OpenSSL digest routines documented at https://www.openssl.org/docs/manmaster/man3/EVP_DigestInit.html


DigestBuilder::DigestBuilder() 
{
    // allocate and initialize the digest-routine context, using the SHA-256 hash function
    mMdCtx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mMdCtx, EVP_sha256(), NULL);

    // allocate and initialize the encode-routing context
    mEncodeCtx = EVP_ENCODE_CTX_new();
    EVP_EncodeInit(mEncodeCtx);
}

DigestBuilder::~DigestBuilder() 
{
    // release the context memory
	EVP_ENCODE_CTX_free(mEncodeCtx);
	EVP_MD_CTX_free(mMdCtx);
}

string DigestBuilder::BuildDigest(const string& message) 
{
    // add the message to the context
	EVP_DigestUpdate(mMdCtx, message.c_str(), message.length());

    // allocate the digest memory and retrieve the encoded hash from the context
	auto digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()));
    unsigned int digestSize;
    EVP_DigestFinal_ex(mMdCtx, digest, &digestSize);
    // at this point, the digest is a "binary" hash in a byte array

    // encode the binary hash in base64, as this how it will be sent in JSON
    unsigned int encodedSize = (int)ceil(4.0 * digestSize / 3.0) + 1;
    unsigned char *encodedDigest = (unsigned char*)OPENSSL_malloc(encodedSize);
    EVP_EncodeBlock(encodedDigest, digest, digestSize);

    // store the output in a string
    string output = string((const char*)encodedDigest);

    // release the allocated memory
    OPENSSL_free(encodedDigest);
    OPENSSL_free(digest);

    return output;
}