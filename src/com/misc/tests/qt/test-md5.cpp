#include "misc/gen/Md5Hash.h"
#include "misc/gen/dbgutils.h"
#include "misc/gen/gen_utils.h"
#include <QCryptographicHash>
#include <cstring>
#include <test/catch.h>

TEST_CASE("sf::md5hash", "[con][generic][md5]")
{
	// Some data to hash and compare having a MD5 == 573be3b9b25fd16a380b88c3911b689c .
	static const char* data = R"(
b=V+lEgKoDM5c3S/cvDIrnPyBuiYFjwpeqWR1jGT02Zh0aQ+UemFmudQMeaObomLZ9pY
QrAQ5rbbO28lCmyby7uLBWNhIZOpz33I1EU7g0L9lWe8oGrcSUF1EPuIQb+7cTFou0l+
TVyuVePHX5gnO8MuxhBfyWrK8T/034bhfG2yHx5DeYj8MaeXjGG78Iu8YR/dgqOr6EH9
2D1WLjz3vjW0/68O/XHe9KCvp3NOWu/o2K7FEVo/Ub3STk57n7Ow0DGRUxyux/Xbf9sx
Yi11kIQ6Gt1RpF3RpYccLiX84L8ltxbfaVH2h5P3ABxZWSehW12MaJX+/MKQalwW0D1C
b=q1nF0XU9rujxmVboCdqIc5WNVfbXU1w2ODkXedkco7toZf8mDI9FbrcRHR48Jz4LI2
SE+kkJ3dRZkmPLByhA8xaeefFIrfkvJt2nB2PKlu4UJvAckoDYVUxZDuiT8Msv3Ci06c
X/J0SFcKJWruVCKf3wLlVrHzsyojbkMQqFuCoKW8rsb8w4WhrOt870hY4k7pRncQq1b4
mcaVn/9d70ZM3VMJxJ4Fr+eDlSy6mnad/p5QKrbmTUG/TGfqsIXYo9dAmsmwsA87AkGi
kbiyXTGzOkottMRT6oHBW4xYaZI452vIhSEDMal3CuPjyBYpeapHe9Mu/u0lekI/F+4/
)";
	static const char* hashString = "573be3b9b25fd16a380b88c3911b689c";

	SECTION("Check to SSL MD5")
	{
		sf::Md5Hash::hash_type hash{};

		//MD5((const unsigned char*) data, strlen(data), hash.data);
		auto qtHashed = QCryptographicHash::hash(QByteArrayView(data), QCryptographicHash::Md5);

		memcpy(hash.data, qtHashed.constData(), sizeof(hash));

		CHECK(sf::hexString(hash.data, sizeof(hash.data)) == hashString);

		CHECK(sf::Md5Hash(data, strlen(data)).hexDigest() == hashString);

		CHECK(sf::Md5Hash(data, strlen(data)).hash() == sf::Md5Hash::from(hashString));

		CHECK(sf::Md5Hash(data, strlen(data)) == sf::Md5Hash(sf::Md5Hash::from(hashString)));
	}
}
