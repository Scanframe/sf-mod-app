#pragma once

#include "../global.h"
#include <cstdio>
#include <cstring>
#include <inttypes.h>
#include <iostream>

namespace sf
{

/**
 * @brief Class replacing obsolete SSL 3.0 library MD5 functionality.
 */
class _MISC_CLASS Md5Hash
{
	public:
		/**
		 * @brief Type for storing an MD5 hash.
		 */
		typedef union
		{
				uint8_t data[16];
				// For faster compare.
				int64_t ints[2];
		} hash_type;

		/** @brief Default constructor. */
		Md5Hash();

		/** @brief Initializing constructor using an unsigned character pointer. */
		explicit Md5Hash(const hash_type& hash);

		/** @brief Initializing constructor using an unsigned character pointer. */
		explicit Md5Hash(const uint8_t* data, size_t length);

		/** Initializing constructor using a character pointer. */
		explicit Md5Hash(const char* data, size_t length);

		/**
		 * @brief Initializing constructor using a std::string.
		 */
		explicit Md5Hash(const std::string& str);

		/**
		 * @brief MD5 block update operation.
		 * Continues an MD5 message-digest operation, processing another message block.
		 */
		void update(const uint8_t* data, size_t length);

		/** @brief MD5 block update operation with unsigned char */
		void update(const char* data, size_t length);

		/** @brief Initialization or reset */
		void initialize();

		/** @brief MD5 finalization.
		 * Ends an MD5 message-digest operation writing the message digest and zeroing the context.
		 */
		Md5Hash& finalize();

		/** @brief  Gets hex representation of digest as string */
		[[nodiscard]] std::string hexDigest() const;

		/** @brief Returns the resulting hash structure/union. */
		[[nodiscard]] hash_type hash() const;

		/**
		 * @brief Get hash_type using the passed hex string.
		 * @param hex_str Hexadecimal string.
		 * @return Initialized hash type.
		 */
		static hash_type from(const std::string& hex_str);

	private:
		/** @brief Constant for the block size. */
		static constexpr size_t blockSize{64};
		/** @brief Flag to check transformation finished */
		bool _finish{false};
		/** @brief 64-bit counter for number of bits(low, high) */
		uint32_t _count[2]{0, 0};
		/** @brief Store A.B.C.D numbers while transforming */
		uint32_t _state[4]{0, 0, 0, 0};
		/** @brief Bytes that didn't fit in last 64 byte chunk. */
		uint8_t _buffer[blockSize]{0};
		/** @brief The result of MD5 */
		hash_type _result{0, 0};

		/** @brief MD5 transform operation. */
		void transform(const uint8_t*);
};

/**
 * @brief Compare operator for storing an MD5 hash.
 */
_MISC_FUNC bool operator==(const Md5Hash::hash_type& h1, const Md5Hash::hash_type& h2);

/**
 * @brief Compare operator for storing an MD5 hash.
 */
inline bool operator==(const Md5Hash& h1, const Md5Hash& h2)
{
	return h1.hash() == h2.hash();
}

/**
 * @brief Stream operator for showing hexadecimal hash value.
 */
inline std::ostream& operator<<(std::ostream& os, const Md5Hash& hash)
{
	return os << hash.hexDigest();
}

/**
 * @brief Read hash from input stream.
 */
_MISC_FUNC std::istream& operator>>(std::istream& is, Md5Hash& h);

}// namespace sf
