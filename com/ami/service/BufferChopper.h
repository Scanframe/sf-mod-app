#pragma once

#include <cstdio>
#include <limits>

namespace sf
{

/**
 * @brief Chops a buffer up in to smaller pieces.
 */
class BufferChopper
{
	public:
		/**
		 * @brief Constructor passing the chop size.
		 * @param chopSize Chop size.
		 */
		explicit BufferChopper(size_t chopSize = 1024);

		/**
		 * @brief Assigns the buffer to be chopped.
		 * @param data Pointer to the data.
		 * @param sz Size of the data pointed at.
		 */
		void assign(char* data, size_t sz);

		/**
		 * @brief Gets the pointer to the next chop of data.
		 * Each call moves the internal pointer up until no more data is available.
		 * @return Pointer to the chop.
		 */
		[[nodiscard]] char* getChunkData();

		/**
		 * @brief Gets the size of the chop pointed at by #getChunkData().
		 * @return Size of the current data chop.
		 */
		[[nodiscard]] size_t getChunkSize() const;

		/**
		 * @brief Moves to the next chuck when available.
		 * @return True on success to do so.
		 */
		bool moveNext();

		/**
		 * @brief Get the status on the chopping being done or not.
		 * @return True when done.
		 */
		[[nodiscard]] bool isDone() const;

	private:
		char* _data{nullptr};
		size_t _size{0};
		size_t _pos{0};
		size_t _chopSize{0};

};

}
