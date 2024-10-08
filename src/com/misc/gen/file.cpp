#include "file.h"
#include "TStrings.h"
#include "dbgutils.h"
#include "gnu_compat.h"
#include "pointer.h"
#include "string.h"
#include <cstring>
#include <dirent.h>
#include <libgen.h>
#if IS_WIN
	#if IS_GNU
		#include <windows.h>
	#else
		#include <Windows.h>
	#endif
#endif

namespace sf
{

std::string getLine(std::istream& is)
{
	char buffer[BUFSIZ];
	is.getline(buffer, sizeof(buffer) - 1);
	return {buffer};
}

std::string fileBaseName(const std::string& path)
{
	std::string p(path);
	return ::basename(const_cast<char*>(p.c_str()));
}

std::string fileDirName(const std::string& path)
{
	std::string p(path);
	return ::dirname(const_cast<char*>(p.c_str()));
}

bool fileUnlink(const std::string& path)
{
#if IS_WIN
	if (::_unlink(path.c_str()) == -1)
#else
	if (::unlink(path.c_str()) == -1)
#endif
	{
		char buffer[BUFSIZ];
		SF_NORM_NOTIFY(DO_DEFAULT, "of '" << path << "' failed!\n"
																			<< ::strerror_r(errno, buffer, sizeof(buffer)))
		return false;
	}
	return true;
}

bool fileRename(const std::string& old_path, const std::string& new_path)
{
	if (std::rename(old_path.c_str(), new_path.c_str()) == -1)
	{
		char buffer[BUFSIZ];
		SF_NORM_NOTIFY(DO_DEFAULT, "from '" << old_path << "' to '" << new_path << "' failed!\n"
																				<< ::strerror_r(errno, buffer, sizeof(buffer)))
		return false;
	}
	return true;
}

bool fileExists(const char* path)
{
#if IS_WIN
	return !::_access(path, F_OK);
#else
	return !::access(path, F_OK);
#endif
}

bool getFiles(strings& files, std::string directory, std::string wildcard)
{
	DIR* dp;
	dirent* dirp;
	// Prevent errors from printing by checking the access.
#if IS_WIN
	if (_access(directory.c_str(), F_OK | X_OK))
#else
	if (access(directory.c_str(), F_OK | X_OK))
#endif
	{
		return false;
	}
	//
	if ((dp = ::opendir(directory.c_str())) == nullptr)
	{
		char buffer[BUFSIZ];
		std::cerr << "Error(" << errno << ") " << strerror_r(errno, buffer, sizeof(buffer)) << " " << directory << std::endl;
		return false;
	}
	//
	while ((dirp = readdir(dp)) != nullptr)
	{
		if (wildcmp(wildcard.c_str(), dirp->d_name, true))
		{
			files.push_back(std::string(dirp->d_name));
		}
	}
	//
	::closedir(dp);
	//
	return true;
}

bool findFiles(sf::strings& files, const std::string& path)
{
	return sf::getFiles(files, fileDirName(path), fileBaseName(path));
}

std::string getTemporaryDirectory()
{
#if IS_WIN
	std::string rv(MAX_PATH, '\0');
	rv.resize(::GetTempPathA(rv.capacity(), rv.data()));
	return rv;
#else
	return {P_tmpdir};
#endif
}

std::string getWorkingDirectory()
{
#if IS_WIN
	constexpr int sz = 4096;
	std::string rv;
	rv.resize(sz, 0);
	rv.resize(strlen(_getcwd(rv.data(), sz)));
	return rv;
#else
	auto* dir = get_current_dir_name();
	std::string rv(dir);
	free(dir);
	return rv;
#endif
}

std::string::value_type getDirectorySeparator()
{
#if IS_WIN
	return '\\';
#else
	return '/';
#endif
}

std::string getExecutableFilepath()
{
#if IS_WIN
	std::string rv(MAX_PATH, '\0');
	rv.resize(::GetModuleFileNameA(nullptr, rv.data(), rv.capacity()));
	return rv;
#else
	std::string rv(PATH_MAX, '\0');
	rv.resize(::readlink("/proc/self/exe", rv.data(), rv.capacity()));
	return rv;
#endif
}

std::string getExecutableDirectory()
{
	auto rv = getExecutableFilepath();
	auto pos = rv.find_last_of(getDirectorySeparator());
	return rv.erase((pos != std::string::npos && pos > 0) ? pos : 0);
}

std::string getExecutableName()
{
	auto rv = getExecutableFilepath();
	auto pos = rv.find_last_of(getDirectorySeparator());
	return rv.erase(0, (pos != std::string::npos && pos > 0) ? pos + 1 : rv.length());
}

}// namespace sf