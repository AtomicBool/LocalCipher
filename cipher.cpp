#include "main.h"

#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

class RSACrypto
{
private:
    BCRYPT_ALG_HANDLE hAlg;
    BCRYPT_KEY_HANDLE hKey;

public:
    RSACrypto() : hAlg(NULL), hKey(NULL)
    {
        NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_ECDH_P256_ALGORITHM, NULL, 0);
		if (status != STATUS_SUCCESS)
        {
            hAlg = NULL;
			printf("[-] Failed to open algorithm provider\n");
        }
    }

    ~RSACrypto()
    {
        if (hKey) BCryptDestroyKey(hKey);
        if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
    }

    bool GenerateKeyPair()
    {
        return BCryptGenerateKeyPair(hAlg, &hKey, 256, 0) == STATUS_SUCCESS;
    }

    bool Encrypt(const std::string& plainText, BYTE** encryptedData, DWORD* encryptedLen) const
    {
        if (!hKey) return false;

        // 将字符串转换为字节
        DWORD dataLen = (DWORD)plainText.length();
        const BYTE* data = (const BYTE*)plainText.c_str();

        // 最多多出255bits
        *encryptedLen = dataLen + 256;
        *encryptedData = new BYTE[*encryptedLen];

        DWORD bytesWritten = 0;
        
        NTSTATUS status = BCryptEncrypt(
            hKey,
            (PUCHAR)data,
            dataLen,
            NULL, NULL, 0,
            *encryptedData,
            *encryptedLen,
            &bytesWritten,
            0
        );

        return status == STATUS_SUCCESS;
    }

    bool Decrypt(BYTE* encryptedData, DWORD encryptedLen, std::string& plainText) const
    {
        if (!hKey) return false;

        // 由于ECC的限制，这里需要更复杂的实现
        // 实际应用中通常使用ECC进行密钥交换，然后用对称加密
        BYTE* outputBuffer = new BYTE[encryptedLen];
        DWORD bytesWritten = 0;

        NTSTATUS status = BCryptDecrypt(
            hKey,
            encryptedData,
            encryptedLen,
            NULL, NULL, 0,
            outputBuffer,
            encryptedLen,
            &bytesWritten,
            0
        );

        if (status == STATUS_SUCCESS)
        {
            plainText.assign((char*)outputBuffer, bytesWritten);
            delete[] outputBuffer;
            return true;
        }

        delete[] outputBuffer;
        return false;
    }

};
