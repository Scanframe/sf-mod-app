#pragma once

#include "../gen/TStrings.h"
#include "../gen/system.h"
#include "../global.h"
#include <fcntl.h>
#include <filesystem>
#include <grp.h>
#include <pwd.h>

namespace sf
{

/**
* Checks if a keyboard key pas pressed.
*/
_MISC_FUNC bool kb_hit();

/**
 * Add missing gettid() system call.
 */
_MISC_FUNC pid_t gettid() noexcept;

/**
 * Returns all the files from the passed directory using a wildcard.
 */
_MISC_FUNC bool getFiles(strings& files, std::string directory, std::string wildcard);

/**
 * @brief Writes a buffer to a filepath.
 * @param path Filepath.
 * @param buf Pointer to the character buffer.
 * @param sz Size of the buffer.
 * @param append When true it appends the buffer to the file.
 * @return
 */
_MISC_FUNC bool file_write(const char* path, const void* buf, size_t sz, bool append = false);

/**
 * @brief Writes a character buffer to file.
 * @param path Filepath.
 * @param buf Pointer to the character buffer.
 * @param sz Size of the buffer.
 * @param append When true it appends the buffer to the file.
* @return True on success.
 */
inline bool file_write(const std::string& path, const void* buf, size_t sz, bool append = false)
{
	return file_write(path.c_str(), buf, sz, append);
}

/**
 * @brief Writes a std::string to file.
 * @param path File path.
 * @param s String to be written to the file.
 * @param sz Size of the buffer.
 * @param append When true it appends the buffer to the file.
 * @return True on success.
 */
inline bool file_write(const std::string& path, const std::string& s, bool append = false)
{
	return file_write(path.c_str(), s.c_str(), s.length(), append);
}

/**
 * Returns the filepath of the passed file descriptor.
 * @param fd Open file descriptor.
 * @return path
 */
_MISC_FUNC std::filesystem::path file_fd_path(int fd);

/**
 * @brief Easier typename for 'struct stat'.
 */
typedef struct stat stat_t;

/**
 * @brief Same as stat() but using a std::string.
 */
_MISC_FUNC bool file_stat(const std::string& path, stat_t& _stat);

/**
 * @brief Formats the time to the given format.
 * @param time_info When NULL the current local time is used.
 * @param format When NULL the XML format is used.
 * @return Formatted time string.
 */
_MISC_FUNC std::string time_format(const struct tm* time_info, const char* format = nullptr);

/**
 * @brief Formats the time to the given format.
 * @param time When -1 the current time is used.
 * @return Formatted time string.
 */
_MISC_FUNC std::string time_format(time_t time = -1, const char* format = nullptr, bool gm_time = false);

/**
 * @brief Gets the unix time from the passed string in the format that was passed.
 * @param format When NULL the XML format is used.
 * @param gm_time When is true GMT is used otherwise the local time.
 * @return Time structure.
 */
_MISC_FUNC time_t time_str2time(const std::string& str, const char* format = nullptr, bool gm_time = false);

/**
 * @brief Same as ::mktime().
 * @param tm Time
 * @param gm_time When true GMT is the result value not the localtime.
 * @return Time as time_t.
 */
_MISC_FUNC time_t time_mktime(struct tm* tm, bool gm_time = false);

/**
 * @brief Makes all directories recursively in the path.
 * @param path Directory path to make.
 * @param mode Mode of each created directory.
 * @return True on success.
 */
_MISC_FUNC bool file_mkdir(const char* path, __mode_t mode = 0755);

/**
 * @brief Makes all directories recursively in the path.
 * @param path Directory path to make.
 * @param mode Mode of each created directory.
 * @return True on success.
 */
inline bool file_mkdir(const std::string& path, __mode_t mode = 0755)
{
	return file_mkdir(path.c_str(), mode);
}

/**
 * Intermediate type to be able to create passwd_t struct/class.
 */
typedef struct passwd passwd_type;

/**
 * @brief Extends struct passwd and auto allocates needed memory.
 * Used as a result type with auto memory cleanup .
 */
struct passwd_t :public passwd_type
{
	public:
		/**
		 * Constructor allocating the right amount of memory.
		 */
		passwd_t();

		/**
		 * Destructor
		 */
		~passwd_t();

		//
		explicit operator bool() const { return valid; }

	private:

		/**
		 * Clears the passwd_type part only.
		 */
		void reset();

		/**
		 * Holds the buffer size.
		 */
		ssize_t buf_sz{0};
		/**
		 * Holds the buffer for the passwd_type dynamic fields.
		 */
		char* buf{nullptr};
		/**
		 * Flag telling if this instance contains data or not.
		 */
		bool valid{};

		friend bool proc_getpwnam(std::string name, passwd_t& pwd);

		friend bool proc_getpwuid(uid_t uid, passwd_t& pwd);
};

/**
 * Wrapper for getpwnam_r() but simplified using passwd_t.
 * Returns true when an entry was found.
 * In case of an error it throws en exception.
 */
bool proc_getpwnam(std::string name, passwd_t& pwd);

/**
 * Wrapper for getpwuid_r() but simplified using passwd_t.
 * Returns true when an entry was found.
 * In case of an error it throws en exception.
 */
bool proc_getpwuid(uid_t uid, passwd_t& pwd);

/**
 * Wrapper for setuid().
 * In case of an error it throws en exception.
 */
void proc_setuid(uid_t uid);

/**
 * Wrapper for seteuid().
 * In case of an error it throws en exception.
 */
void proc_seteuid(uid_t uid);

/**
 * Intermediate type to be able to create group_t struct/class.
 */
typedef struct group group_type;

/**
 * Extends struct group and auto allocates needed memory.
 * Used as a result type with auto memory cleanup .
 */
struct group_t :public group_type
{
	public:
		/**
		 * Constructor allocating the right amount of memory.
		 */
		group_t();

		/**
		 * Destructor
		 */
		~group_t();

		/**
		 * Boolean operator returning validity.
		 */
		explicit operator bool() const
		{
			return valid;
		}

	private:
		/**
		 * Clears the passwd_type part only.
		 */
		void reset();

		/**
		 * Holds the buffer size.
		 */
		ssize_t buf_sz{0};
		/**
		 * Holds the buffer for the group_type dynamic fields.
		 */
		char* buf{nullptr};
		/**
		 * Flag telling if this instance contains data or not.
		 */
		bool valid{false};

		friend bool proc_getgrnam(const std::string& name, group_t& grp);

		friend bool proc_getgrgid(gid_t gid, group_t& grp);
};

/**
 * Wrapper for getgrnam_r() but simplified using group_t.
 * Returns true when an entry was found.
 * In case of an error it throws en exception.
 */
_MISC_FUNC bool proc_getgrnam(const std::string& name, group_t& grp);

/**
 * Wrapper for getpwuid_r() but simplified using group_t.
 * Returns true when an entry was found.
 * In case of an error it throws en exception.
 */
_MISC_FUNC bool proc_getgrgid(gid_t gid, group_t& grp);

/**
 * Wrapper for setgid().
 * In case of an error it throws en exception.
 */
_MISC_FUNC void proc_setgid(gid_t gid);

/**
 * Wrapper for setegid().
 * In case of an error it throws en exception.
 */
_MISC_FUNC void proc_setegid(gid_t gid);

/**
 * Wrapper for setfsuid().
 * In case of an error it throws en exception.
 */
_MISC_FUNC void proc_setfsuid(uid_t uid);

/**
 * Wrapper for setfsgid().
 * In case of an error it throws en exception.
 */
_MISC_FUNC void proc_setfsgid(gid_t gid);

/**
 * @brief Replacement for deprecated '::siginterrupt' function.
 * Returns EINVAL when the sig argument is not a valid signal number otherwise zero.
 * @param sig A signal like defined in the SA_????? defines.
 * @param flag Boolean value for installing and uninstalling the signal.
 */
int _MISC_FUNC siginterrupt(int sig, int flag);

}// namespace sf
