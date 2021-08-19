#include <fstream>
#include <algorithm>

#include "gen_utils.h"
#include "TDynamicBuffer.h"
#include "DynamicLibraryInfo.h"

namespace sf
{

DynamicLibraryInfo::DynamicLibraryInfo(const DynamicLibraryInfo& dld) = default;

void DynamicLibraryInfo::clear()
{
	directory.clear();
	filename.clear();
	name.clear();
	description.clear();
}

std::string DynamicLibraryInfo::path() const
{
	std::string::value_type dir_sep;
#if IS_WIN
	dir_sep = '\\';
#else
	dir_sep = '/';
#endif
	return directory + dir_sep + filename;
}

bool DynamicLibraryInfo::read(const std::string& dir, const std::string& fn)
{
	std::string::value_type dir_sep;
	#if IS_WIN
	dir_sep = '\\';
#else
	dir_sep = '/';
#endif
	std::string filepath = dir + dir_sep + fn;
	// clear the structure members.
	clear();
	// Get the markers to look for.
	const uint64_t mark_beg = *((uint64_t*) (SF_DL_MARKER_BEGIN));
	const uint64_t mark_end = *((uint64_t*) (SF_DL_MARKER_END));
	// Set to 1 when the begin marker has been found and to 2 when both are found.
	int flag = 0;
	// Sanity check.
	if (!fileExists(filepath))
	{
		return false;
	}
	// Create a 16 KByte buffer.
	DynamicBuffer buf(16 * 1024);
	// Create the stream.
	std::ifstream is(filepath.c_str(), std::ios::binary | std::ios::in);
	// Sanity check on if it is open.
	if (!is.is_open())
	{
		return false;
	}
	std::ifstream::off_type beg_idx = 0, end_idx = 0;
	// get length of file:
	is.seekg(0, std::ifstream::end);
	std::size_t len = is.tellg();
	// Move reading pointer to the start in the stream.
	is.seekg(0, std::ifstream::beg);
	// Holds end read index .
	std::size_t ofs_idx = 0;
	// Buffer can not be bigger then the filesize.
	if (buf.size() > len)
	{
		buf.resize(len, true);
	}
	// Loop until end of file.
	do
	{
		// Read binary data into the buffer.
		is.read(buf.c_str(), buf.reserved());
		// Check the file.
		if (!is.good())
		{
			is.close();
			return false;
		}
		// Set the virtual size of the buffer so the last 64 bit integer can be compared.
		buf.resize(1 + is.tellg() - ofs_idx - sizeof(uint64_t));
		// Iterate through the buffer comparing markers.
		for (std::size_t i = 0; i < buf.size(); i++)
		{
			// When the begin or end marker matches.
			if (*(uint64_t*) buf.data(i) == (flag ? mark_end : mark_beg))
			{
				// When the begin marker has been found.
				if (!flag)
				{
					// Set the flag to find the end mark.
					flag = 1;
					// Calculate the last index of the begin marker which starts at after the matching 64bit position.
					beg_idx = i + ofs_idx + sizeof(uint64_t);
				}
					// End marker has also be found.
				else
				{
					// Set the flag to stop the loop.
					flag = 2;
					// Calculate the last index of the end marker.
					end_idx = i + ofs_idx;
					// Break the loop.
					break;
				}
			}
		}
		// When end of file break the loop.
		if (!is.eof())
		{
			// Increment the offset index with the size of the buffer which is the file chunk that was compared.
			ofs_idx += buf.size();
			// Move file read pointer some bytes back for the markers to compare fully next round.
			is.seekg(1 - sizeof(uint64_t), std::ifstream::cur);
		}
	}
	while (!is.eof() && flag <= 1);
	// We found a match.
	if (flag > 1)
	{
		// Set get index on the stream.
		is.seekg(beg_idx, std::ifstream::beg);
		// Resize the buffer to accommodate the size between the markers and the needed terminating zero.
		buf.resize(end_idx - beg_idx);
		// Read the data between the markers into the buffer.
		is.read(buf.c_str(), buf.size());
		//
		if (is.good())
		{
			// Terminate the character string.
			buf[buf.size()] = '\0';
			//
			char* sep = strstr(buf.c_str(), SF_DL_NAME_SEPARATOR);
			if (sep != nullptr)
			{
				// Terminate the name.
				*sep = '\0';
				// Move the pointer to the start of the description.
				sep += strlen(SF_DL_NAME_SEPARATOR);
				// Assign the name part.
				name = buf.c_str();
				// Assign the description part.
				description = sep;
				// Assign the directory.
				directory = dir;
				// Assign the name.
				filename = fn;
			}
		}
	}
	// Close the file stream.
	is.close();
	// Signal failure.
	return false;
}

} // namespace sf
