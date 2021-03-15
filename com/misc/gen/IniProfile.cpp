#include <string>
#include <cstring>
#include <cstdlib>

#include "gen_utils.h"
#include "IniProfile.h"

namespace sf
{

//
// Helper functions
//
std::istream& skip_empty_lines(std::istream& is)
{ // now skip all new line character or carriage returns
	while (is.good() && strchr("\n\r", is.peek()))
	{
		is.get();
	}
	return is;
}

std::istream& skip_to_nextline(std::istream& is)
{
	// find next new line character or carriage return.
	while (is.good() && !strchr("\n\r", is.peek()))
	{
		is.get();
	}
	// now skip all new line character or carriage returns.
	return skip_empty_lines(is);
}

std::istream& read_to_delim(std::string& s, std::istream& is, int delim)
{
	// Set the chunk amount of data to read.
	const std::string::size_type increment = 256;
	char buffer[increment + 10];
	// InitializeBase the read counter.
//	std::string::size_type rc = 0;
	s.clear();
	// skip first character if this is a delimiter
	if (is.peek() == delim)
	{
		is.get();
	}
	// Keep reading chunks until the delimiter is found.
	while (is.good() && is.peek() != delim)
	{
//		s.resize(increment + rc + 10);
//		is.get((char*)(s.c_str() + rc), increment, delim);
//		rc += is.gcount();
		is.get(buffer, increment, delim);
		s.append(buffer, is.gcount());
	}
	// Give the string the correct size.
//	s.resize(rc);
	// Return the passed stream.
	return is;
}

//
// IniProfile Members
//

void IniProfile::Initialize()
{
	SectionListPtr = EntryListType::npos;
}

IniProfile::IniProfile(bool skip_init)
	:SectionList(0)
	 , Dirty(false)
	 , FlagClearOnRead(true)
	 , SectionListPtr(UINT_MAX)
	 , WriteOnDirty(true)
{
	Initialize();
	if (!skip_init)
	{
		Init(nullptr);
	}
}

IniProfile::IniProfile()
	:SectionList(0)
	 , Dirty(false)
	 , FlagClearOnRead(true)
	 , WriteOnDirty(true)
	 , SectionListPtr(UINT_MAX)
{
	Initialize();
	Init(nullptr);
}

IniProfile::IniProfile(const char* path)
	:SectionList(0)
	 , Dirty(false)
	 , FlagClearOnRead(true)
	 , WriteOnDirty(true)
	 , SectionListPtr(UINT_MAX)
{
	Initialize();
	Init(path);
}

IniProfile::IniProfile(const char* section, const char* path)
	:SectionList(0)
	 , Dirty(false)
	 , FlagClearOnRead(true)
	 , WriteOnDirty(true)
	 , SectionListPtr(UINT_MAX)
{
	Initialize();
	Init(path);
	SetSection(section);
}

IniProfile::IniProfile(std::istream& is)
	:SectionList(0)
	 , Dirty(false)
	 , FlagClearOnRead(true)
	 , WriteOnDirty(true)
	 , SectionListPtr(UINT_MAX)
{
	Initialize();
	Init(is);
}

IniProfile::~IniProfile()
{
	// Before destruction write all changes to file, if there is a change and it is allowed.
	if (WriteOnDirty)
	{
		Write();
	}
	// Clear all entries.
	Flush();
}

void IniProfile::Flush()
{
	// Delete all section instances.
	for (auto i: SectionList)
	{
		delete i;
	}
	// Flush the pointers in the list.
	SectionList.flush();
}

bool IniProfile::Init(const char* path)
{  // create path to profile file
	// if path = NULL the startup directory is selected with default name
	if (path)
	{
		Path = path;
	}
	else
	{
		Path += "default.cfg";
	}
	//
	bool rv = true;
	// create 'std::istream' using Path member variable
	if (Path.length())
	{
		std::istream* is = new std::ifstream(Path.c_str(), std::ios::in/*, filebuf::openprot*/);
		rv = Init(*is);
		delete_null(is);
	}
	return rv;
}

bool IniProfile::Init(std::istream& is)
{
	// Set dirty flag to false
	Dirty = false;
	// Read profile from stream
	bool retval = Read(is);
	// Check size of profile and Set current section ptr
	SectionListPtr = SectionList.count() ? 0 : UINT_MAX;
	return retval;
}

void IniProfile::SetKeyPrefix(const char* prefix)
{
	Prefix = prefix ? prefix : "";
}

bool IniProfile::Read(std::istream& is)
{
	while (is.good())
	{  // create empty section
		auto section = new Section;
		// read the next section
		if (section->Read(is))
		{
			// add section on success
			SectionList.add(section);
		}
		else
		{
			// delete section on failure
			delete_null(section);
		}
	}
	// if eof flag was reason for stopping reading return true as well
	return (is.good() || is.eof());
}

std::ostream& IniProfile::Write(std::ostream& os) const
{
	if (os.good())
	{
		for (const auto& i: SectionList)
		{
			i->Write(os) << '\n';
		}
	}
	return os;
}

bool IniProfile::Write()
{
	// check dirty flag and Path member
	if (Dirty && Path.length())
	{
		// clear file upon opening
		std::ostream* os = new std::ofstream(Path.c_str(), std::ios::out | std::ios::trunc);
		// after file is written without errors Dirty flag is Set to false
		Dirty = !Write(*os).good();
		// write End Of File (EOF) character to the end
		//os->put('\x1A');
		// Cleanup.
		delete os;
	}
	// if not dirty file is written well or wasn't dirty
	return !Dirty;
}

const char* IniProfile::GetSection(IniProfile::size_type p) const
{
	// return NULL if there are no section loaded at all
	if (SectionList.count())
	{ // check if 'p' has a valid value
		if (p < SectionList.count() || p == npos)
		{
			return SectionList[(p == npos) ? SectionListPtr : p]->Name.c_str();
		}
	}
	return nullptr;
}

bool IniProfile::SetSection(IniProfile::size_type p)
{
	// Return NULL if there are no section loaded at all
	if (SectionList.count())
	{
		// Check if 'p' has a valid value
		if (p < SectionList.count())
		{
			SectionListPtr = p;
			return true;
		}
	}
	// On an invalid value keep current SectionListPtr
	return false;
}

bool IniProfile::SetSection(const char* section, bool create)
{
	// Locate section
	SectionListPtr = FindSection(section);
	// If found return true
	if (SectionListPtr != UINT_MAX)
	{
		return true;
	}
	// Check if section must and can be created
	if (create && section)
	{
		// Create empty section
		auto s = new Section();
		// Assign name to it
		s->Name = section;
		// Add section to the list
		SectionList.add(s);
		// Make this section the current one
		SectionListPtr = SectionList.count() - 1;
		// Set dirty flag
		Dirty = true;
		// Tells that section dit not exist at the start of this function
		return false;
	}
	// If section not found make first section the current one
	SectionListPtr = 0;
	// tells that section dit not exist at the start of this function
	return false;
}

IniProfile::size_type  IniProfile::FindEntry(const char* key)
{
	auto rv = SectionList.count() ? SectionList[SectionListPtr]->FindEntry(key) : IniProfile::npos;
	if (rv == IniProfile::npos)
	{
		if (SectionList.count() && key)
		{
			std::clog << "IniProfile: Key '" << key << "' Not Found In Section '"
				<< SectionList[SectionListPtr]->Name << "' In File '" << Path << std::endl;
		}
	}
	return rv;
}

bool IniProfile::GetString(const char* key, char buff[], size_t buffSize, const char* defaultString) const
{
	IniProfile::size_type p = SectionList[SectionListPtr]->FindEntry(key);
	if (p == IniProfile::npos)
	{
		// If not found, copy default string into buffer and return false.
		// Check for valid buffer.
		if (buff)
		{
			strncpy(buff, defaultString ? defaultString : "", buffSize);
		}
		return false;
	}
	// Check for valid buffer.
	if (buff)
	{
		strncpy(buff, SectionList[SectionListPtr]->EntryList[p]->GetValue(), buffSize);
	}
	return true;
}

bool IniProfile::GetString(const char* key, std::string& value, const char* defaultString) const
{
	auto p = SectionList[SectionListPtr]->FindEntry(key);
	if (p == SectionListType::npos)
	{
		// if not found, copy default std::string into the std::string and return false
		// Check for valid buffer
		value = defaultString ? defaultString : "";
		// Signal failure.
		return false;
	}
	// Check for valid buffer
	value = SectionList[SectionListPtr]->EntryList[p]->GetValue();
	// Signal success.
	return true;
}

std::string IniProfile::GetString(const char* key, const char* defaultString) const
{
	std::string value;
	GetString(key, value, defaultString);
	return value;
}

int IniProfile::GetInt(const char* key, int defaultInt) const
{
	const char* value = nullptr;
	// check section entries
	if (SectionList.count())
	{
		// get current section entry value, returns NULL when not found
		value = SectionList[SectionListPtr]->GetEntry(key, nullptr);
	}
	// if value == NULL return default value passed to function
	return value ? atoi(value) : defaultInt; // NOLINT(cert-err34-c)
}

bool IniProfile::SetString(const char* key, const char* value)
{  // check section entry count
	if (SectionList.count())
	{  // Set dirty flag so the write function really writes to file
		Dirty = true;
		// look key up in current section and Set the entry
		return SectionList[SectionListPtr]->SetEntry(key, value);
	}
	return false;
}

bool IniProfile::SetInt(const char* key, int value)
{
	// Use SetString function to do this
	char buf[33];
	return SetString(key, itoa(value, buf, 10));
}

IniProfile::Entry* IniProfile::GetEntry(IniProfile::size_type p)
{
	// Check for sections
	if (SectionList.count())
	{
		// Get section pointer
		Section* section = SectionList[SectionListPtr];
		// Check for valid index
		if (p < section->EntryList.count())
		{ // Return entry pointer
			return section->EntryList[p];
		}
	}
	// return entry pointer NULL if not exist
	return nullptr;
}

bool IniProfile::InsertComment(const char* key, const char* comment)
{
	// Check section entry count.
	if (SectionList.count())
	{
		// Set dirty flag so the write function really writes to file.
		Dirty = true;
		// Look key up in current section and Set the entry.
		return SectionList[SectionListPtr]->InsertComment(key, comment);
	}
	return false;
}

IniProfile::size_type IniProfile::FindSection(const char* section)
{
	if (!section)
	{
		return SectionListPtr;
	}
	// Loop through section list
	for (IniProfile::size_type i = 0; i < SectionList.count(); i++)
	{
		// Section name is case insensitive compared
		if (!::strcmp(SectionList[i]->Name.c_str(), section))
		{
			// Return position
			return i;
		}
	}
	// Return UINT_MAX on not found
	return IniProfile::npos;
}

bool IniProfile::RemoveSection(IniProfile::size_type p)
{ // check for valid parameters
	if (p != IniProfile::npos && p < SectionList.count())
	{ // delete instance
		delete_null(SectionList[p]);
		// remove from list
		SectionList.detach(p);
		// correct current section index pointer
		if (p < SectionListPtr)
		{
			SectionListPtr--;
		}
		if (SectionListPtr >= SectionList.count())
		{
			SectionListPtr = SectionList.count() - 1;
		}
		// Set dirty flag
		Dirty = true;
		// return true on success
		return true;
	}
	return false;
}

//=============================================================================
// Section Members
//=============================================================================
//
IniProfile::Section::~Section()
{
	EntryListType::iter_type i(EntryList);
	while (i)
	{
		delete (i++);
	}
}

bool IniProfile::Section::Read(std::istream& is)
{
	// check stream for errors
	if (is.good())
	{
		// find section begin, is the first '[' character on a line
		while (is.good() && is.get() != '[')
		{
			is.ignore(INT_MAX, '\n');
		}
		// test stream for errors
		if (is.good())
		{
			read_to_delim(Name, is, ']');
			// skip to end of this line
			skip_to_nextline(is);
			// check for section start character
			while (is.good() && is.peek() != '[')
			{
				// create empty entry
				auto entry = new Entry();
				// read entry from if stream has no errors and entry
				if (entry->Read(is) && entry->IsValid())
				{
					EntryList.add(entry);
				}
				else
				{
					delete_null(entry);
				}
				// skip empty lines
				skip_empty_lines(is);
			}
			// read of Section suceeded
			return true;
		}
	}
	// error reading Section
	return false;
}

std::ostream& IniProfile::Section::Write(std::ostream& os) // NOLINT(readability-make-member-function-const)
{
	// Check stream for errors
	if (os.good())
	{
		// Write section head
		os << '[' << Name << ']' << '\n';
		EntryListType::iter_type i(EntryList);
		// write all entries to the stream
		while (i)
		{
			(i++)->Write(os);
		}
	}
	return os;
}

const char* IniProfile::Section::GetEntry(const char* key, const char* defValue)
{
	auto p = FindEntry(key);
	return (p == EntryListType::npos) ? defValue : EntryList[p]->GetValue();
}

bool IniProfile::Section::SetEntry(const char* key, const char* value)
{
	// Check for valid parameters.
	if (key && value && strlen(key))
	{ // create the entry
		auto entry = new Entry(key, value);
		// When not found p is equals EntryListType::npos.
		auto p = FindEntry(key);
		// if entry was found
		if (p != EntryListType::npos)
		{
			// Remove entry from heap
			delete_null(EntryList[p]);
			// Assign new entry instance pointer to same location in list
			EntryList[p] = entry;
		}
		// if not found
		else
		{
			// Add entry at the end of the section list
			EntryList.add(entry);
		}
		// Signal success.
		return true;
	}
	// Signal failure.
	return false;
}

bool IniProfile::Section::InsertComment(const char* key, const char* comment)
{
	// Check for valid parameters
	if (key && comment && strlen(key))
	{
		// Create the comment entry
		auto entry = new Entry(comment);
		// If not found p is equals UINT_MAX
		auto p = FindEntry(key);
		// When found
		if (p != EntryListType::npos)
		{
			// Add entry at found location
			EntryList.addAt(entry, p);
			// Signal success.
			return true;
		}
	}
	// Comment was not inserted
	return false;
}

IniProfile::EntryListType::size_type IniProfile::Section::FindEntry(const char* key)
{
	// Check if key is valid.
	if (key)
	{
		// Loop through entry list
		for (EntryListType::size_type p = 0; p < EntryList.count(); p++)
		{
			// Section name is case sensitive compared.
			if (!::strcmp(EntryList[p]->GetKey(), key))
			{
				// return pointer in list
				return p;
			}
		}
	}
	// Signal not found.
	return EntryListType::npos;
}

bool IniProfile::Section::RemoveEntry(EntryListType::size_type p)
{ // check for valid parameters
	if (p != EntryListType::npos && p < EntryList.count())
	{
		// delete entry instance
		delete_null(EntryList[p]);
		// remove from list
		EntryList.detach(p);
		// return true on success
		return true;
	}
	return false;
}

//=============================================================================
// Entry Members
//=============================================================================
//
IniProfile::Entry::Entry(const char* key, const char* value)
	:Line(nullptr)
	 , ValPos(0)
{
	Set(std::string(key).append("=").append(value));
}

IniProfile::Entry::Entry(const char* comment)
	:Line(nullptr)
	 , ValPos(0)
{
	// make comment line
	Set(std::string("; ").append(comment));
}

IniProfile::Entry::~Entry()
{
	free_null(Line);
}

bool IniProfile::Entry::Set(const std::string& line)
{
	delete_null(Line);
	// create entry line buffer
	Line = (char*) malloc(line.length() + 1);
	// fill the Line member with data
	strncpy(Line, line.c_str(), line.length() + 1);
	// terminate line
	Line[line.length()] = 0;
	// check if this line is a comment line, line starts with ';' character
	if (Line[0] != ';')
	{
		// get seperator position
		ValPos = line.find_first_of('=');
		// if not found the line is the keyword itself ValPos==NPOS
		if (ValPos != std::string::npos)
		{
			// Save value position position
			std::string::size_type p = ValPos;
			// Separate key and value by placing a terminator
			Line[ValPos++] = 0;
			// Skip white space after key part of std::string
			while (Line[--p] == ' ' && p)
			{
				Line[p] = 0;
			}
			// Check for valid key
			if (!p)
			{
				// Free and clear because pointer is also flag
				free_null(Line);
			}
			else
			{
				// Skip white space in front of value.
				while (Line[ValPos] == ' ')
				{
					Line[ValPos++] = 0;
				}
			}
		}
		else
		{
			// Free and clear because pointer is also flag
			free_null(Line);
		}
	}
	else
	{
		// if it is a comment line Set ValPos to zero
		ValPos = 0;
	}
	// if Line has a value the function succeeded
	return Line != nullptr;
}

bool IniProfile::Entry::Read(std::istream& is)
{
#if 1
	std::string line;
	getline(is, line);
	// When the end of the stream has been reached a string is not empty signal succes.
	if (line.length() && is.eof())
	{
		return Set(line);
	}
	return is.good() && Set(line);
#else
	// set skip_whitespace flag to true
	int prev_skip_value = std::string::skip_whitespace(1);
	std::string line;
	// use std::string as buffer to read line from stream
	// skips white space at the beginning, depends on mstring::skip_white
	line.read_line(is);
	// turn back skip whitespace
	std::string::skip_whitespace(prev_skip_value);
	// if read went well return value of Set function if not return false
	return is.good() ? Set(line) : false;
#endif
}

const char* IniProfile::Entry::GetKey()
{ // check if line is valid
	return (Line && ValPos) ? Line : "";
}

const char* IniProfile::Entry::GetValue()
{
	// Check if line is valid
	return Line ? &Line[ValPos] : "";
}

std::ostream& IniProfile::Entry::Write(std::ostream& os)
{ // check stream for errors
	if (os.good())
	{
		if (ValPos)
		{
			os << GetKey() << '=';
		}
		os << GetValue() << '\n';
	}
	return os;
}

} // namespace sf
