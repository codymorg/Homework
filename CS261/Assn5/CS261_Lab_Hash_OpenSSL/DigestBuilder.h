#include <openssl/evp.h>

using namespace std;  // not needed in RoboCatAction

class DigestBuilder 
{
public:
    DigestBuilder();
    ~DigestBuilder();

    string BuildDigest(const string& message);

private:
    EVP_MD_CTX *mMdCtx;
    EVP_ENCODE_CTX *mEncodeCtx;
};