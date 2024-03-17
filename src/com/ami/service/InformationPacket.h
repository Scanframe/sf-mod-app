#pragma once

#include <misc/gen/TDynamicBuffer.h>

namespace sf
{

/**
 * @brief Packet manipulation class.
 */
class InformationPacket
{
	public:
		/**
		 * @brief Enumeration packet types used in the packet header.
		 */
		enum EType :uint8_t
		{
			/**
			 * @brief Send version to compare get same version and pass what data to subscribe to.
			 */
			tInitialize = 0,
			/**
			 * @brief Sends a packet and it should return it only incrementing the counter.
			 */
			tPingPong,
			tVariableInfo,
			tVariable,
			tResultData,
		};

		/**
		 * @brief Default constructor.
		 */
		InformationPacket() = default;

		/**
		 * @brief Default constructor.
		 */
		static size_t headerSize()
		{
			return sizeof(Header);
		}

		/**
		 * @brief Allocates the memory needed for the packet.
		 */
		template<typename T>
		T* allocatePayload(EType type)
		{
			_buffer.resize(sizeof(Header) + sizeof(T));
			auto* hdr = _buffer.ptr<Header>();
			hdr->size = _buffer.size();
			hdr->type = type;
			return _buffer.ptr<T>(sizeof(Header));
		}

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
			 * @brief size of the data attached in the payload.
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
		struct PayloadPingPong
		{
			/**
			 * @brief Counter to be decremented by 1 by the server and send back and vise versa until the counter is zero.
			 */
			int32_t counter{0};
			/**
			 * @brief Some null terminated string.
			 */
			char str[64]{};
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
		 * @brief Gets the header of the current packet.
		 */
		[[nodiscard]] Header& getHeader()
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

		/**
		 * @brief Gets a typed payload of the current packet.
		 */
		template<typename T>
		T& getPayload()
		{
			assert(getHeader().size < sizeof(T));
			return *_buffer.ptr<T>(sizeof(Header));
		}

		/**
		 * @brief Gets the data pointer of the buffer.
		 */
		[[nodiscard]] const char* data() const
		{
			return _buffer.ptr<char>();
		}

		/**
		 * @brief Gets the data pointer of the buffer.
		 */
		[[nodiscard]] char* data()
		{
			return _buffer.ptr<char>();
		}

		/**
		 * @brief Gets the size of the packet buffer.
		 */
		[[nodiscard]] size_t size() const
		{
			return _buffer.size();
		}

		/**
		 * @brief Resizes the buffer to include the .
		 */
		void resizePayload(size_t sz)
		{
			return _buffer.resize(sizeof(Header) + sz);
		}

	private:
		// Holds the actual data and initial size if the header size.
		DynamicBuffer _buffer{sizeof(Header)};
};

}