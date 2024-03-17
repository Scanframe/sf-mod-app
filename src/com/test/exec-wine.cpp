#include <stdlib.h>
#include <iostream>

std::string findAndReplaceAll(std::string data, std::string_view toSearch, std::string_view replaceStr)
{
	// Get the first occurrence
	size_t pos = data.find(toSearch);
	// Repeat till end is reached
	while (pos != std::string::npos)
	{
		// Replace this occurrence of Sub String
		data.replace(pos, toSearch.size(), replaceStr);
		// Get the next occurrence from the current position
		pos = data.find(toSearch, pos + replaceStr.size());
	}
	return data;
}

std::string escapeShellArg(std::string str)
{
	str = findAndReplaceAll(str, "\\", "\\\\");
	str = findAndReplaceAll(str, "\'", "\\'");
	if (str.find(' ') != std::string::npos)
	{
		str = std::string(1, '\'').append(str).append(1, '\'');
	}
	return str;
}

std::string dirnameOf(const std::string& filepath)
{
	size_t pos = filepath.find_last_of("\\/");
	if (std::string::npos == pos)
		return {};
	return filepath.substr(0, pos);
}

int executeWine(int argc, char* argv[])
{
	std::string args = escapeShellArg(dirnameOf(argv[0]).append("/../win-exec.sh"));
	for (int j = 1; j < argc; j++)
	{
		args.append(" ").append(escapeShellArg(argv[j]));
	}
	if (argc == 1)
	{
		std::cout << args << std::endl;
		return EXIT_SUCCESS;
	}
	return system(args.c_str());
}

int main(int argc, char* argv[])
{
	return executeWine(argc, argv);
}
