#include "cipher.h"
#include <wincrypt.h>

#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "crypt32.lib")

RSAProcessor::RSAProcessor() {
    BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_RSA_ALGORITHM, NULL, 0);
}

RSAProcessor::~RSAProcessor() {
    if (hKey) BCryptDestroyKey(hKey);
    if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
}

std::string RSAProcessor::Base64Encode(const std::vector<BYTE>& data) {
    if (data.empty()) return "";
    DWORD len = 0;
    CryptBinaryToStringA(data.data(), (DWORD)data.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &len);
    std::vector<char> buffer(len);
    CryptBinaryToStringA(data.data(), (DWORD)data.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, buffer.data(), &len);
    return std::string(buffer.data());
}

std::vector<BYTE> RSAProcessor::Base64Decode(const std::string& base64) {
    if (base64.empty()) return {};
    DWORD len = 0;
    CryptStringToBinaryA(base64.c_str(), 0, CRYPT_STRING_BASE64, NULL, &len, NULL, NULL);
    std::vector<BYTE> buffer(len);
    CryptStringToBinaryA(base64.c_str(), 0, CRYPT_STRING_BASE64, buffer.data(), &len, NULL, NULL);
    return buffer;
}

bool RSAProcessor::GenerateKeyPair(std::string& pubKeyHex, std::string& privKeyHex) {
    if (!hAlg) return false;
    if (BCryptGenerateKeyPair(hAlg, &hKey, 2048, 0) != 0) return false;
    if (BCryptFinalizeKeyPair(hKey, 0) != 0) return false;
    pubKeyHex = ExportKey(BCRYPT_RSAPUBLIC_BLOB);
    privKeyHex = ExportKey(BCRYPT_RSAFULLPRIVATE_BLOB);
    return true;
}

std::string RSAProcessor::ExportKey(LPCWSTR blobType) {
    if (!hKey) return "";
    DWORD size = 0;
    if (BCryptExportKey(hKey, NULL, blobType, NULL, 0, &size, 0) != 0) return "";
    std::vector<BYTE> buffer(size);
    if (BCryptExportKey(hKey, NULL, blobType, buffer.data(), size, &size, 0) != 0) return "";
    return Base64Encode(buffer);
}

bool RSAProcessor::ImportKey(const std::string& base64, LPCWSTR blobType) {
    if (!hAlg) return false;
    if (hKey) { BCryptDestroyKey(hKey); hKey = NULL; }
    std::vector<BYTE> buffer = Base64Decode(base64);
    if (buffer.empty()) return false;
    return BCryptImportKeyPair(hAlg, NULL, blobType, &hKey, buffer.data(), (DWORD)buffer.size(), 0) == 0;
}

std::string RSAProcessor::Encrypt(const std::string& plainText) {
    if (!hKey) return "";
    BCRYPT_PKCS1_PADDING_INFO padInfo = { NULL }; // Standard PKCS1 padding
    DWORD size = 0;
    NTSTATUS status = BCryptEncrypt(hKey, (PUCHAR)plainText.c_str(), (DWORD)plainText.length(), &padInfo, NULL, 0, NULL, 0, &size, BCRYPT_PAD_PKCS1);
    if (status != 0) return "";
    
    std::vector<BYTE> buffer(size);
    status = BCryptEncrypt(hKey, (PUCHAR)plainText.c_str(), (DWORD)plainText.length(), &padInfo, NULL, 0, buffer.data(), size, &size, BCRYPT_PAD_PKCS1);
    if (status != 0) return "";
    
    return Base64Encode(buffer);
}

std::string RSAProcessor::Decrypt(const std::string& cipherBase64) {
    if (!hKey) return "";
    std::vector<BYTE> cipher = Base64Decode(cipherBase64);
    if (cipher.empty()) return "";
    
    BCRYPT_PKCS1_PADDING_INFO padInfo = { NULL };
    DWORD size = 0;
    NTSTATUS status = BCryptDecrypt(hKey, cipher.data(), (DWORD)cipher.size(), &padInfo, NULL, 0, NULL, 0, &size, BCRYPT_PAD_PKCS1);
    if (status != 0) return "";
    
    std::vector<BYTE> buffer(size);
    status = BCryptDecrypt(hKey, cipher.data(), (DWORD)cipher.size(), &padInfo, NULL, 0, buffer.data(), size, &size, BCRYPT_PAD_PKCS1);
    if (status != 0) return "";
    
    return std::string((char*)buffer.data(), size);
}
