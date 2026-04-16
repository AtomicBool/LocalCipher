#include "core/Application.h"

#include "utils/os/file.h"
#include "utils/os/conversion.h"

void rsa_demo() {
    RSA rsa_usr, rsa_peer;

    const std::string privatePath = "private.key";
    const std::string publicPath = "public.key";

    bool loaded = false;

    if (FileUtils::Exists(privatePath))
    {
        auto privateKey = FileUtils::LoadBinary(privatePath);

        if (!privateKey.empty())
        {
            loaded = rsa_usr.ImportPrivateKey(privateKey);

            if (loaded)
            {
                std::cout << "Loaded existing keypair\n";
            }
        }
    }

    if (!loaded)
    {
        std::cout << "Generating new keypair...\n";

        if (!rsa_usr.GenerateKeyPair())
        {
            std::cout << "Key generation failed\n";
            return;
        }

        auto publicKey = rsa_usr.ExportPublicKey();
        auto privateKey = rsa_usr.ExportPrivateKey();

        FileUtils::SaveBinary(publicPath, publicKey);
        FileUtils::SaveBinary(privatePath, privateKey);

        std::cout << "New keypair generated and saved\n";
    }

    std::string msg =
        "你说的对，但是《原神》是由米哈游自主研发的一款全新开放世界冒险游戏。"
        "因为你的素养很差，我现在每天玩原神都能赚150原石，每个月差不多5000原石的收入，"
        "也就是现实生活中每个月5000美元的收入水平，换算过来最少也30000人民币，虽然我只有14岁，"
        "但是已经超越了中国绝大多数人（包括你）的水平，这便是原神给我的骄傲的资本。"
        "一个不玩原神的人，有两种可能性。一种是没有能力玩原神。因为买不起高配的手机和抽不起卡等各种自身因素，"
        "他的人生都是失败的，第二种可能：有能力却不玩原神的人，在有能力而没有玩原神的想法时，"
        "那么这个人的思想境界便低到了一个令人发指的程度。一个有能力的人不付出行动来证明自己，"
        "只能证明此人行为素质修养之低下。是灰暗的，是不被真正的上流社会认可的。原神真的特别好玩，"
        "不玩的话就是不爱国，因为原神是国产之光，原神可惜就在于它是国产游戏，如果它是一款国外游戏的话，"
        "那一定会比现在还要火。如果你要是喷原神的话那你一定是tx请的水军。玩游戏，不玩原神，那玩什么呢？"
        "没错，无非就是鬼泣、塞尔达，血源诅咒；这样不爱国的玩家，素质修养真的很低";

    auto encrypted = rsa_usr.Encrypt(msg);

    std::cout << "Encrypted size: "
        << encrypted.size()
        << std::endl;

    std::cout << "Encrypted data: "
        << Conversion::BytesToString(encrypted)
        << std::endl;

    // Decrypt
    std::string decrypted = rsa_usr.Decrypt(encrypted);

    std::cout << "rsa_user decryption: "
        << decrypted
        << std::endl;

    auto publicKey = rsa_usr.ExportPublicKey();
    std::string publicHex = Conversion::BytesToString(publicKey);

    std::cout << "rsa_user Public Key: " << publicHex << std::endl;

    auto pubBytes = Conversion::StringToBytes(publicHex);
    rsa_peer.ImportPublicKey(pubBytes);

    auto peer_encrypted = Conversion::BytesToString(rsa_peer.Encrypt(msg));

    std::cout << "rsa_peer encryption:" << peer_encrypted << std::endl;

    std::cout << "rsa_user - peer decryption: " << rsa_usr.Decrypt(Conversion::StringToBytes(peer_encrypted)) << std::endl;
}

int main(int, char**)
{
    rsa_demo();

    Application app;

    if (!app.Initialize()) {
        return 1;
    }

    app.Run();

    return 0;
}
