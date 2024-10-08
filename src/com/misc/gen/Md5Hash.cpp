#include "Md5Hash.h"
#include "string.h"

namespace sf
{

// Anonymous namespace to prevent collisions.
namespace
{

// Number used while transforming in functions FF, GG, HH, II.
enum
{
	S11 = 7,
	S12 = 12,
	S13 = 17,
	S14 = 22,
	S21 = 5,
	S22 = 9,
	S23 = 14,
	S24 = 20,
	S31 = 4,
	S32 = 11,
	S33 = 16,
	S34 = 23,
	S41 = 6,
	S42 = 10,
	S43 = 15,
	S44 = 21
};

// F, G， H，I are the basic MD5 functions
inline uint32_t F(uint32_t x, uint32_t y, uint32_t z)
{
	return x & y | ~x & z;
}

inline uint32_t G(uint32_t x, uint32_t y, uint32_t z)
{
	return x & z | y & ~z;
}

inline uint32_t H(uint32_t x, uint32_t y, uint32_t z)
{
	return x ^ y ^ z;
}

inline uint32_t I(uint32_t x, uint32_t y, uint32_t z)
{
	return y ^ (x | ~z);
}

// Rotates x left n bits
inline uint32_t rotateLeft(uint32_t x, size_t n)
{
	return (x << n) | (x >> (32 - n));
}

// FF, GG, HH, II transformations for rounds 1, 2, 3, 4.
inline void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, size_t s, uint32_t ac)
{
	a = rotateLeft(a + F(b, c, d) + x + ac, s) + b;
}

inline void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, size_t s, uint32_t ac)
{
	a = rotateLeft(a + G(b, c, d) + x + ac, s) + b;
}

inline void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, size_t s, uint32_t ac)
{
	a = rotateLeft(a + H(b, c, d) + x + ac, s) + b;
}

inline void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, size_t s, uint32_t ac)
{
	a = rotateLeft(a + I(b, c, d) + x + ac, s) + b;
}

// Decodes uint8_t into uint32_t. Assumes length is 64.
inline void md5_decode(uint32_t* output, const uint8_t* input)
{
	for (uint32_t i = 0, j = 0; j < 64; i++, j += 4)
	{
		output[i] = (uint32_t) input[j] | ((uint32_t) input[j + 1] << 8) | ((uint32_t) input[j + 2] << 16) | ((uint32_t) input[j + 3] << 24);
	}
}

// Encodes uint32_t into uint8_t. Assumes length is a multiple of 4
inline void md5_encode(uint8_t* output, const uint32_t* input, size_t length)
{
	for (uint32_t i = 0, j = 0; j < length; i++, j += 4)
	{
		output[j] = input[i] & 0xff;
		output[j + 1] = (input[i] >> 8) & 0xff;
		output[j + 2] = (input[i] >> 16) & 0xff;
		output[j + 3] = (input[i] >> 24) & 0xff;
	}
}

}// namespace

Md5Hash::Md5Hash()
{
	initialize();
}

Md5Hash::Md5Hash(const uint8_t* data, size_t length)
{
	initialize();
	update(data, length);
	finalize();
}

Md5Hash::Md5Hash(const char* data, size_t length)
{
	initialize();
	update(data, length);
	finalize();
}

Md5Hash::Md5Hash(const std::string& str)
{
	initialize();
	update(str.data(), str.length());
	finalize();
}

void Md5Hash::update(const uint8_t* input, size_t length)
{
	// Compute number of bytes mod 64
	uint32_t index = _count[0] / 8 % blockSize;
	// Update number of bits
	if ((_count[0] += (length << 3)) < (length << 3))
	{
		_count[1]++;
	}
	_count[1] += (length >> 29);
	// Number of bytes we need to fill in buffer
	uint32_t firstPart = 64 - index;
	uint32_t i;
	// Transform as many times as possible
	if (length >= firstPart)
	{
		// Fill buffer first, transform
		memcpy(&_buffer[index], input, firstPart);
		transform(_buffer);
		// Transform chunks of blockSize(64)
		for (i = firstPart; i + blockSize <= length; i += blockSize)
		{
			transform(&input[i]);
		}
		index = 0;
	}
	else
	{
		i = 0;
	}
	// Buffer remaining input
	memcpy(&_buffer[index], &input[i], length - i);
}

void Md5Hash::update(const char* input, std::size_t length)
{
	update((const uint8_t*) input, length);
}

Md5Hash& Md5Hash::finalize()
{
	static uint8_t padding[64] =
		{
			0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};
	if (!_finish)
	{
		// Save number of bits
		uint8_t bits[8];
		md5_encode(bits, _count, 8);
		// Pad out to 56 mod 64
		uint32_t index = _count[0] / 8 % 64;
		uint32_t padLength = (index < 56) ? (56 - index) : (120 - index);
		update(padding, padLength);
		// Append length (before padding)
		update(bits, 8);
		// Store state in result
		md5_encode(_result.data, _state, 16);
		// Set finish flag.
		_finish = true;
	}
	return *this;
}

std::string Md5Hash::hexDigest() const
{
	if (!_finish)
	{
		return {};
	}
	return hexString(&_result, sizeof(_result));
}

std::ostream& operator<<(std::ostream& os, const Md5Hash md5)
{
	return os << md5.hexDigest();
}

void Md5Hash::initialize()
{
	_finish = false;
	_count[0] = _count[1] = 0;
	// Magic number
	_state[0] = 0x67452301;
	_state[1] = 0xefcdab89;
	_state[2] = 0x98badcfe;
	_state[3] = 0x10325476;
}

void Md5Hash::transform(const uint8_t block[blockSize])
{
	uint32_t a = _state[0];
	uint32_t b = _state[1];
	uint32_t c = _state[2];
	uint32_t d = _state[3];
	uint32_t x[16];
	md5_decode(x, block);
	// Round One
	FF(a, b, c, d, x[0], S11, 0xd76aa478);
	FF(d, a, b, c, x[1], S12, 0xe8c7b756);
	FF(c, d, a, b, x[2], S13, 0x242070db);
	FF(b, c, d, a, x[3], S14, 0xc1bdceee);
	FF(a, b, c, d, x[4], S11, 0xf57c0faf);
	FF(d, a, b, c, x[5], S12, 0x4787c62a);
	FF(c, d, a, b, x[6], S13, 0xa8304613);
	FF(b, c, d, a, x[7], S14, 0xfd469501);
	FF(a, b, c, d, x[8], S11, 0x698098d8);
	FF(d, a, b, c, x[9], S12, 0x8b44f7af);
	FF(c, d, a, b, x[10], S13, 0xffff5bb1);
	FF(b, c, d, a, x[11], S14, 0x895cd7be);
	FF(a, b, c, d, x[12], S11, 0x6b901122);
	FF(d, a, b, c, x[13], S12, 0xfd987193);
	FF(c, d, a, b, x[14], S13, 0xa679438e);
	FF(b, c, d, a, x[15], S14, 0x49b40821);
	// Round Two
	GG(a, b, c, d, x[1], S21, 0xf61e2562);
	GG(d, a, b, c, x[6], S22, 0xc040b340);
	GG(c, d, a, b, x[11], S23, 0x265e5a51);
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);
	GG(a, b, c, d, x[5], S21, 0xd62f105d);
	GG(d, a, b, c, x[10], S22, 0x2441453);
	GG(c, d, a, b, x[15], S23, 0xd8a1e681);
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);
	GG(a, b, c, d, x[9], S21, 0x21e1cde6);
	GG(d, a, b, c, x[14], S22, 0xc33707d6);
	GG(c, d, a, b, x[3], S23, 0xf4d50d87);
	GG(b, c, d, a, x[8], S24, 0x455a14ed);
	GG(a, b, c, d, x[13], S21, 0xa9e3e905);
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8);
	GG(c, d, a, b, x[7], S23, 0x676f02d9);
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);
	// Round Three
	HH(a, b, c, d, x[5], S31, 0xfffa3942);
	HH(d, a, b, c, x[8], S32, 0x8771f681);
	HH(c, d, a, b, x[11], S33, 0x6d9d6122);
	HH(b, c, d, a, x[14], S34, 0xfde5380c);
	HH(a, b, c, d, x[1], S31, 0xa4beea44);
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9);
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60);
	HH(b, c, d, a, x[10], S34, 0xbebfbc70);
	HH(a, b, c, d, x[13], S31, 0x289b7ec6);
	HH(d, a, b, c, x[0], S32, 0xeaa127fa);
	HH(c, d, a, b, x[3], S33, 0xd4ef3085);
	HH(b, c, d, a, x[6], S34, 0x4881d05);
	HH(a, b, c, d, x[9], S31, 0xd9d4d039);
	HH(d, a, b, c, x[12], S32, 0xe6db99e5);
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8);
	HH(b, c, d, a, x[2], S34, 0xc4ac5665);
	// Round Four
	II(a, b, c, d, x[0], S41, 0xf4292244);
	II(d, a, b, c, x[7], S42, 0x432aff97);
	II(c, d, a, b, x[14], S43, 0xab9423a7);
	II(b, c, d, a, x[5], S44, 0xfc93a039);
	II(a, b, c, d, x[12], S41, 0x655b59c3);
	II(d, a, b, c, x[3], S42, 0x8f0ccc92);
	II(c, d, a, b, x[10], S43, 0xffeff47d);
	II(b, c, d, a, x[1], S44, 0x85845dd1);
	II(a, b, c, d, x[8], S41, 0x6fa87e4f);
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0);
	II(c, d, a, b, x[6], S43, 0xa3014314);
	II(b, c, d, a, x[13], S44, 0x4e0811a1);
	II(a, b, c, d, x[4], S41, 0xf7537e82);
	II(d, a, b, c, x[11], S42, 0xbd3af235);
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb);
	II(b, c, d, a, x[9], S44, 0xeb86d391);
	// Add back to the A.B.C.D
	_state[0] += a;
	_state[1] += b;
	_state[2] += c;
	_state[3] += d;
}

Md5Hash::hash_type Md5Hash::hash() const
{
	return _result;
}

Md5Hash::hash_type Md5Hash::from(const std::string& hex_str)
{
	Md5Hash::hash_type h;
	if (hex_str.length() >= sizeof(_result.data) * 2)
	{
		stringHex(hex_str.data(), h.data, sizeof(h.data));
	}
	return h;
}

Md5Hash::Md5Hash(const Md5Hash::hash_type& hash)
{
	_result = hash;
	_finish = true;
}

bool operator==(const Md5Hash::hash_type& h1, const Md5Hash::hash_type& h2)
{
	// Use the unions fast compare integers.
	return h1.ints[0] == h2.ints[0] && h1.ints[1] == h2.ints[1];
}

std::istream& operator>>(std::istream& is, Md5Hash::hash_type& h)
{
	char buf[sizeof(h) * 2 + 1];
	// Get the need.
	is.read(buf, sizeof(h.data) * 2);
	// Clear the hash.
	memset(h.data, 0, sizeof(h.data));
	// Check the stream.
	if (!is.bad())
	{
		stringHex(buf, h.data, sizeof(h.data));
	}
	// Returns the input stream.
	return is;
}

}// namespace sf