#pragma once

#include <fcntl.h>
#include <ftw.h>
#include <filesystem>
#include "../gen/TDynamicBuffer.h"
#include "../global.h"

namespace sf::lnx
{

/**
 * Implements a file handling class around a file descriptor.
 */
class _MISC_CLASS File
{
	public:
		/**
		 * Local simple type definition for statistics.
		 */
		typedef struct stat stat_type;

		/**
		 * Default constructor.
		 */
		File();

		/**
		 * Initialization constructor.
		 */
		explicit File(const std::filesystem::path& path, int flags = O_CREAT | O_RDWR | O_APPEND,
			mode_t mode = S_IREAD | S_IWRITE | S_IRGRP | S_IWGRP);

		/**
		 * Destructor.
		 */
		~File();

		/**
		 * Initializes this instance without opening the actual file
		 * or closes an open file. Throws an exception on failure.
		 */
		void initialize(const std::filesystem::path& path, int flags = O_CREAT | O_RDWR | O_APPEND,
			mode_t mode = S_IREAD | S_IWRITE | S_IRGRP | S_IWGRP);

		/**
		 * Opens the file using the path, flags and mode.
		 * Throws an exception on failure.
		 */
		void open(const std::string& path, int flags = O_CREAT | O_RDWR | O_APPEND,
			mode_t mode = S_IREAD | S_IWRITE | S_IRGRP | S_IWGRP);

		/**
		 * Opens the file using a template path with flags.
		 * Flags O_CREAT and O_RDWR are always implied.
		 * Throws an exception on failure.
		 */
		void createTemporary(const std::string& name_tpl, int flags = O_CREAT | O_RDWR);

		/**
		 * Allocates disk space for the file.
		 * @param sz
		 */
		void allocate(size_t sz);

		/**
		 * Opens a closed file using the same parameters.
		 * Throws an exception on failure.
		 */
		void open(bool reopen = false);

		/**
		 * Checks if the file is opened.
		 * @return True when the file is open.
		 */
		[[nodiscard]] bool isOpen() const;

		/**
		 * When open it closes the file.
		 * Throws an exception on failure to close when open.
		 */
		void close();

		/**
		 * When exceptions flag is false no exceptions are thrown.
		 * Useful when closing a file in destructors or exception cleanup.
		 */
		bool close(bool exceptions);

		/**
		 * Writes data from the passed file into this file.
		 * When the buffer 'buf_sz' size is zero a default value is used
		 * in which the data is copied in chunks.
		 */
		void write(const File& f, size_t pos, size_t sz, size_t buf_sz = 0);

		/**
		 * Writes a buffer to a file descriptor reporting errors when they occurred.
		 * Throws an exception on an error.
		 */
		void write(const void* buf, size_t sz, size_t* written = nullptr);

		void write(const std::string& s);

		void write(const char* s);

		/**
		 * Write template function
		 * @tparam T Type of the structure written.
		 * @param t
		 */
		template<class T>
		void write(const T& t);

		/**
		 * Reads from the file and returns the actual amount read.
		 * Throws an exception in case of an error.
		 */
		ssize_t read(void* buf, size_t pos, size_t sz) const;

		/**
		 * Reads data into a dynamic buffer,
		 * @param buf
		 * @param pos
		 * @param sz
		 */
		void read(DynamicBuffer buf, size_t pos, size_t sz) const;

		/**
		 * Reads data into a structure.
		 * @tparam T Type of the structure.
		 * @param t
		 * @param pos File position.
		 */
		template<class T>
		void read(T& t, size_t pos) const;

		/**
		 * Returns the stats of the file descriptor.
		 * Throws an exception on an error.
		 */
		void getStat(stat_type& s) const;

		/**
		 * Returns the stat info structure on the opened file.
		 */
		[[nodiscard]] const stat_type& getStatus() const;

		/**
		 * Returns the stat info structure on the opened file.
		 * When update is true the data is refreshed.
		 * Throws an exception on an error on doing so.
		 */
		const stat_type& getStatus(bool update);

		/**
		 * Returns the TStats struct as a meaningful text.
		 */
		[[nodiscard]] std::string getStatText() const;

		/**
		 * Rename the current file and updates the path member when assign is true.
		 * Throws an exception on an error on doing so.
		 */
		void rename(const std::string& path, bool assign = false);

		/**
		 * Truncates the file and can be performed on the open or closed file.
		 * Throws an exception on an error.
		 */
		void truncate(size_t length);

		/**
		 * Closes an open file and Removes/Unlinks the current file.
		 * Throws an exception on an error on doing so.
		 */
		void remove();

		/**
		 * Syncs the current file.
		 */
		void synchronise();

		/**
		 * Returns the path of the opened file.
		 */
		[[nodiscard]] std::string getPath() const;

		/**
		 * Returns the mode of the opened file.
		 */
		[[nodiscard]] mode_t getMode() const;

		/**
		 * Returns the flags of the opened files.
		 */
		[[nodiscard]] int getFlags() const;

		/**
		 * Returns the file descriptor.
		 */
		[[nodiscard]] int getDescriptor() const;

		/**
		 * Check if the file exist.
		 * @return
		 */
		bool exists();

		/**
		 * Removes the file wen it exist.
		 */
		void unlink();

	private:
		/**
		 * Assign the last error number.
		 */
		int setErrorNo(int error_no);
		/**
		 * Holds the full file path of the opened file.
		 */
		std::filesystem::path _path;
		/**
		 * Holds the open flags.
		 */
		mode_t _mode{};
		/**
		 * Holds the open mode.
		 */
		mode_t _flags{};
		/**
		 * Holds the file descriptor.
		 */
		int _descriptor{};
		/**
		 * Holds the last error.
		 */
		int _errorNo{};
		/**
		 * Holds the recent stat info.
		 */
		stat_type _stat{};
};

inline
int File::getDescriptor() const
{
	return _descriptor;
}

inline
std::string File::getPath() const
{
	return _path;
}

inline
mode_t File::getMode() const
{
	return _mode;
}

inline
int File::getFlags() const
{
	return _flags;
}

inline
void File::close()
{
	close(true);
}

inline
const File::stat_type& File::getStatus() const
{
	return _stat;
}

inline
const File::stat_type& File::getStatus(bool update)
{
	if (update)
	{
		getStat(_stat);
	}
	return _stat;
}

inline
void File::write(const std::string& s)
{
	write(s.c_str(), s.length());
}

inline
void File::write(const char* s)
{
	write(s, strlen(s));
}

template<class T>
void File::write(const T& t)
{
	write(&t, sizeof(T));
}

template<class T>
void File::read(T& t, size_t pos) const
{
	read(&t, pos, sizeof(T));
}

inline
bool File::isOpen() const
{
	return _descriptor != -1;
}

/**
 * Out stream operator for the TFile class.
 */
_MISC_FUNC std::ostream& operator<<(std::ostream& stream, const File& file);

}