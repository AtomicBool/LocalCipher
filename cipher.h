#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <bcrypt.h>

class RSAProcessor {
public:
    static std::string Base64Encode(const std::vector<BYTE>& data);
    static std::vector<BYTE> Base64Decode(const std::string& base64);

    BCRYPT_KEY_HANDLE hKey = NULL;
    BCRYPT_ALG_HANDLE hAlg = NULL;

    RSAProcessor();
    ~RSAProcessor();

    bool GenerateKeyPair(std::string& pubKeyHex, std::string& privKeyHex);
    std::string ExportKey(LPCWSTR blobType);
    bool ImportKey(const std::string& base64, LPCWSTR blobType);
    std::string Encrypt(const std::string& plainText);
    std::string Decrypt(const std::string& cipherBase64);
};
