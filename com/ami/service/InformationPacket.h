#pragma once

#include <misc/gen/TDynamicBuffer.h>

namespace sf
{

/**
 * @brief Packet manipulation class.
 */
class Packet
{
	public:
		/**
		 * @brief Enumeration packet types used in the packet header.
		 */
		enum EType : uint8_t
		{
			/**
			 * Send version to compare get same version and pass what data to subscribe to.
			 */
			tInitialize = 0,
			/**
			 * Sends a packet and it should return it only incrementing the counter.
			 */
			tPingPong = 0,
			tVariableInfo,
			tVariable,
			tResultData,
		};

// Make the compiler use 1 byte alignment.
#pragma pack(push, 1)

		/**
		 * @brief Byte aligned packet header.
		 */
		struct Header
		{
			/**
			 * @brief Type of the header.
			 */
			EType type{};
			/**
			 * @brief size of the data attached.
			 */
			uint32_t size{0};
			/**
			 * @brief Sequence number for tracking order of handling while debugging.
			 */
			uint32_t sequence{0};
		};

		/**
		 * @brief Packet payload for #tInitialize type.
		 */
		struct PayloadInitialize
		{
			/**
			 * @brief Null terminated version number.
			 */
			char version[32]{};
			/**
			 * @brief Flag for subscribing to variables.
			 */
			uint8_t subscribeToVariables{0};
			/**
			 * @brief Flag for subscribing to result-data.
			 */
			uint8_t subscribeToResultData{0};
		};

		/**
		 * @brief Packet payload for #tPingPong type.
		 */
		struct PayloadPing
		{
			/**
			 * @brief Counter to be incremented by 1 by the server and send back.
			 */
			uint32_t counter{0};
		};

// Restore compiler previous alignment.
#pragma pack(pop)

		/**
		 * @brief Gets the header of the current packet.
		 */
		[[nodiscard]] const Header& getHeader() const
		{
			return *_buffer.ptr<Header>();
		}

		/**
		 * @brief Gets a typed payload of the current packet.
		 */
		template<typename T>
		const T& getPayload() const
		{
			assert(getHeader().size < sizeof(T));
			return *_buffer.ptr<T>(sizeof(Header));
		}

	private:
		// Holds the actual data and initial size if the header size.
		DynamicBuffer _buffer{sizeof(Header)};
};

}