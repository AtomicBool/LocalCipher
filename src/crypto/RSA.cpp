#include "crypto/RSA.h"

RSA::RSA()
{
    BCryptOpenAlgorithmProvider(
        &m_alg,
        BCRYPT_RSA_ALGORITHM,
        nullptr,
        0
    );
}

RSA::~RSA()
{
    if (m_publicKey) BCryptDestroyKey(m_publicKey);
    if (m_privateKey) BCryptDestroyKey(m_privateKey);
    if (m_alg) BCryptCloseAlgorithmProvider(m_alg, 0);
}

// =====================================================
// KEY GENERATION
// =====================================================
bool RSA::GenerateKeyPair(int keySize)
{
    if (!m_alg) return false;

    if (BCryptGenerateKeyPair(m_alg, &m_privateKey, keySize, 0) != 0)
        return false;

    if (BCryptFinalizeKeyPair(m_privateKey, 0) != 0)
        return false;

    // derive public key
    m_publicKey = m_privateKey;

    return true;
}

// =====================================================
// ENCRYPT (chunked)
// =====================================================
std::vector<uint8_t> RSA::Encrypt(const std::string& data)
{
    std::vector<uint8_t> result;

    size_t blockSize = 190; // safe for 2048-bit RSA PKCS1 padding

    for (size_t i = 0; i < data.size(); i += blockSize)
    {
        size_t len = min(blockSize, data.size() - i);

        auto block = EncryptBlock(
            (const uint8_t*)data.data() + i,
            len
        );

        result.insert(result.end(), block.begin(), block.end());
    }

    return result;
}

// =====================================================
// DECRYPT (chunked)
// =====================================================
std::string RSA::Decrypt(const std::vector<uint8_t>& data)
{
    std::string result;

    size_t blockSize = 256; // RSA 2048 output size

    for (size_t i = 0; i < data.size(); i += blockSize)
    {
        size_t len = min(blockSize, data.size() - i);

        auto block = DecryptBlock(data.data() + i, len);

        result.append(block.begin(), block.end());
    }

    return result;
}

// =====================================================
// BLOCK ENCRYPT
// =====================================================
std::vector<uint8_t> RSA::EncryptBlock(const uint8_t* data, size_t size)
{
    ULONG outSize = 0;

    BCryptEncrypt(
        m_publicKey,
        (PUCHAR)data,
        (ULONG)size,
        nullptr,
        nullptr,
        0,
        nullptr,
        0,
        &outSize,
        BCRYPT_PAD_PKCS1
    );

    std::vector<uint8_t> out(outSize);

    BCryptEncrypt(
        m_publicKey,
        (PUCHAR)data,
        (ULONG)size,
        nullptr,
        nullptr,
        0,
        out.data(),
        outSize,
        &outSize,
        BCRYPT_PAD_PKCS1
    );

    return out;
}

// =====================================================
// BLOCK DECRYPT
// =====================================================
std::vector<uint8_t> RSA::DecryptBlock(const uint8_t* data, size_t size)
{
    ULONG outSize = 0;

    BCryptDecrypt(
        m_privateKey,
        (PUCHAR)data,
        (ULONG)size,
        nullptr,
        nullptr,
        0,
        nullptr,
        0,
        &outSize,
        BCRYPT_PAD_PKCS1
    );

    std::vector<uint8_t> out(outSize);

    BCryptDecrypt(
        m_privateKey,
        (PUCHAR)data,
        (ULONG)size,
        nullptr,
        nullptr,
        0,
        out.data(),
        outSize,
        &outSize,
        BCRYPT_PAD_PKCS1
    );

    return out;
}

// =====================================================
// EXPORT (simple blob)
// =====================================================
std::vector<uint8_t> RSA::ExportPublicKey()
{
    ULONG size = 0;

    BCryptExportKey(
        m_publicKey,
        nullptr,
        BCRYPT_PUBLIC_KEY_BLOB,
        nullptr,
        0,
        &size,
        0
    );

    std::vector<uint8_t> blob(size);

    BCryptExportKey(
        m_publicKey,
        nullptr,
        BCRYPT_PUBLIC_KEY_BLOB,
        blob.data(),
        size,
        &size,
        0
    );

    return blob;
}

// =====================================================
std::vector<uint8_t> RSA::ExportPrivateKey()
{
    ULONG size = 0;

    BCryptExportKey(
        m_privateKey,
        nullptr,
        BCRYPT_RSAFULLPRIVATE_BLOB,
        nullptr,
        0,
        &size,
        0
    );

    std::vector<uint8_t> blob(size);

    BCryptExportKey(
        m_privateKey,
        nullptr,
        BCRYPT_RSAFULLPRIVATE_BLOB,
        blob.data(),
        size,
        &size,
        0
    );

    return blob;
}

// =====================================================
// IMPORT
// =====================================================
bool RSA::ImportPublicKey(const std::vector<uint8_t>& blob)
{
    if (BCryptImportKeyPair(
        m_alg,
        nullptr,
        BCRYPT_PUBLIC_KEY_BLOB,
        &m_publicKey,
        (PUCHAR)blob.data(),
        (ULONG)blob.size(),
        0
    ) != 0)
        return false;

    return true;
}

bool RSA::ImportPrivateKey(const std::vector<uint8_t>& blob)
{
    if (BCryptImportKeyPair(
        m_alg,
        nullptr,
        BCRYPT_RSAFULLPRIVATE_BLOB,
        &m_privateKey,
        (PUCHAR)blob.data(),
        (ULONG)blob.size(),
        0
    ) != 0)
        return false;

    return true;
}