#pragma once

#include <cstdlib>
namespace sf
{

/**
 * @brief Stitches a buffer from a multiple of writes.
 */
class BufferStitcher
{
	public:
		BufferStitcher() = default;

		/**
		 * @brief assign the destination buffer and the size expected.
		 *
		 * @param data Pointer to the buffer.
		 * @param sz Size of the buffer.
		 */
		void assign(char* data, size_t sz);

		/**
		 * @brief Gets the pointer into the buffer to write next.
		 */
		[[nodiscard]] char* getWritePointer() const;

		/**
		 * @brief Gets the size that still needs to be written/stitched.
		 */
		[[nodiscard]] size_t getWriteSize() const;

		/**
		 * @brief Moves the write position up for the next write.
		 * @param sz
		 * @return True when
		 */
		bool movePosition(size_t sz);

		/**
		 * @brief Gets the status of whether all data has been stitched together.
		 * When not initialized.
		 * @return True when all stitching has been done.
		 */
		bool isDone();

	private:
		// Holds the pointer to the data.
		char* _data{nullptr};
		// Holds the size of the buffer.
		size_t _size{0};
		// Holds the position where to write into the buffer.
		size_t _pos{0};
};

}// namespace sf