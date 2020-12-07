#include <string>
#include <cstring>
#include <cstdlib>

#include "genutils.h"
#include "profile.h"

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
	// Initialize the read counter.
//	size_t rc = 0;
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
// TIniProfile Members
//

void TIniProfile::Initialize()
{
	SectionListPtr = UINT_MAX;
}

TIniProfile::TIniProfile(bool skip_init)
	: SectionList(0)
	, Dirty(false)
	, FlagClearOnRead(true)
	,SectionListPtr(UINT_MAX)
	,WriteOnDirty(true)
{
	Initialize();
	if (!skip_init)
	{
		Init(nullptr);
	}
}

TIniProfile::TIniProfile()
	: SectionList(0)
	, Dirty(false)
	, FlagClearOnRead(true)
	, WriteOnDirty(true)
	, SectionListPtr(UINT_MAX)
{
	Initialize();
	Init(nullptr);
}

TIniProfile::TIniProfile(const char* path)
	: SectionList(0)
	, Dirty(false)
	, FlagClearOnRead(true)
	, WriteOnDirty(true)
	, SectionListPtr(UINT_MAX)
{
	Initialize();
	Init(path);
}

TIniProfile::TIniProfile(const char* section, const char* path)
	: SectionList(0)
	, Dirty(false)
	, FlagClearOnRead(true)
	, WriteOnDirty(true)
	, SectionListPtr(UINT_MAX)
{
	Initialize();
	Init(path);
	SetSection(section);
}

TIniProfile::TIniProfile(std::istream& is)
	: SectionList(0)
	, Dirty(false)
	, FlagClearOnRead(true)
	, WriteOnDirty(true)
	, SectionListPtr(UINT_MAX)
{
	Initialize();
	Init(is);
}

TIniProfile::~TIniProfile()
{
	// Before destruction write all changes to file, if there is a change and it is allowed.
	if (WriteOnDirty)
	{
		Write();
	}
	// Clear all entries.
	Flush();
}

void TIniProfile::Flush()
{
	TSectionListIterator i(SectionList);
	// Delete all section instances.
	while (i)
	{
		delete (i++);
	}
	// Flush the pointers in the list.
	SectionList.Flush();
}

bool TIniProfile::Init(const char* path)
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
	bool retval = true;
	// create 'std::istream' using Path member variable
	if (Path.length())
	{
		std::istream* is = new std::ifstream(Path.c_str(), std::ios::in/*, filebuf::openprot*/);
		retval = Init(*is);
		delete_null(is);
	}
	return retval;
}

bool TIniProfile::Init(std::istream& is)
{
	// set dirty flag to false
	Dirty = false;
	// read profile from stream
	bool retval = Read(is);
	// check size of profile and set current section Ptr
	SectionListPtr = SectionList.Count() ? 0 : UINT_MAX;
	return retval;
}

void TIniProfile::SetKeyPrefix(const char* prefix)
{
	Prefix = prefix ? prefix : "";
}

bool TIniProfile::Read(std::istream& is)
{
	while (is.good())
	{  // create empty section
		auto section = new TSection;
		// read the next section
		if (section->Read(is))
		{
			// add section on success
			SectionList.Add(section);
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

std::ostream& TIniProfile::Write(std::ostream& os) const
{
	if (os.good())
	{ //write all sections to the stream
		TSectionListIterator i(SectionList);
		while (i)
		{
			// write section to stream and add a empty line at the end
			i.Current()->Write(os) << '\n';
			i++;
		}
	}
	return os;
}

bool TIniProfile::Write()
{
	// check dirty flag and Path member
	if (Dirty && Path.length())
	{
		// clear file upon opening
		std::ostream* os = new std::ofstream(Path.c_str(), std::ios::out | std::ios::trunc);
		// after file is written without errors Dirty flag is set to false
		Dirty = !Write(*os).good();
		// write End Of File (EOF) character to the end
		//os->put('\x1A');
		// Cleanup.
		delete os;
	}
	// if not dirty file is written well or wasn't dirty
	return !Dirty;
}

const char* TIniProfile::GetSection(unsigned p) const
{
	// return NULL if there are no section loaded at all
	if (SectionList.Count())
	{ // check if 'p' has a valid value
		if (p < SectionList.Count() || p == UINT_MAX)
		{
			// if p==UINT_MAX get current section
			return SectionList[(p == UINT_MAX) ? SectionListPtr : p]->Name.c_str();
		}
	}
	return nullptr;
}

bool TIniProfile::SetSection(unsigned p)
{
	// return NULL if there are no section loaded at all
	if (SectionList.Count())
	{ // check if 'p' has a valid value
		if (p < SectionList.Count())
		{
			SectionListPtr = p;
			return true;
		}
	}
	// on an invalid value keep current SectionListPtr
	return false;
}

bool TIniProfile::SetSection(const char* section, bool create)
{
	// locate section
	SectionListPtr = FindSection(section);
	// if found return true
	if (SectionListPtr != UINT_MAX)
	{
		return true;
	}
	// check if section must and can be created
	if (create && section)
	{ // create empty section
		auto s = new TSection();
		// assign name to it
		s->Name = section;
		// add section to the list
		SectionList.Add(s);
		// make this section the current one
		SectionListPtr = SectionList.Count() - 1;
		// set dirty flag
		Dirty = true;
		// tells that section dit not exist at the start of this function
		return false;
	}
	// if section not found make first section the current one
	SectionListPtr = 0;
	// tells that section dit not exist at the start of this function
	return false;
}

unsigned TIniProfile::FindEntry(const char* key)
{
	unsigned retval = SectionList.Count() ? SectionList[SectionListPtr]->FindEntry(key) : UINT_MAX;
	if (retval == UINT_MAX)
	{
		if (SectionList.Count() && key)
		{
			std::clog << "TIniProfile: Key '" << key << "' Not Found In Section '"
								<< SectionList[SectionListPtr]->Name << "' In File '" << Path << std::endl;
		}
	}
	return retval;
}

bool TIniProfile::GetString(const char* key, char buff[], unsigned buffSize, const char* defaultString) const
{
	unsigned p = SectionList[SectionListPtr]->FindEntry(key);
	if (p == UINT_MAX)
	{ // if not found, copy default string into buffer and return false
		// check for valid buffer
		if (buff)
		{
			strncpy(buff, defaultString ? defaultString : "", buffSize);
		}
		return false;
	}
	// check for valid buffer
	if (buff)
	{
		strncpy(buff, SectionList[SectionListPtr]->EntryList[p]->GetValue(), buffSize);
	}
	return true;
}

bool TIniProfile::GetString(const char* key, std::string& value, const char* defaultString) const
{
	unsigned p = SectionList[SectionListPtr]->FindEntry(key);
	if (p == UINT_MAX)
	{ // if not found, copy default std::string into the std::string and return false
		// check for valid buffer
		value = defaultString ? defaultString : "";
		return false;
	}
	// check for valid buffer
	value = SectionList[SectionListPtr]->EntryList[p]->GetValue();
	return true;
}

std::string TIniProfile::GetString(const char* key, const char* defaultString) const
{
	std::string value;
	GetString(key, value, defaultString);
	return value;
}

int TIniProfile::GetInt(const char* key, int defaultInt) const
{
	const char* value = nullptr;
	// check section entries
	if (SectionList.Count())
	{
		// get current section entry value, returns NULL when not found
		value = SectionList[SectionListPtr]->GetEntry(key, nullptr);
	}
	// if value == NULL return default value passed to function
	return value ? atoi(value) : defaultInt; // NOLINT(cert-err34-c)
}

bool TIniProfile::SetString(const char* key, const char* value)
{  // check section entry count
	if (SectionList.Count())
	{  // set dirty flag so the write function really writes to file
		Dirty = true;
		// look key up in current section and set the entry
		return SectionList[SectionListPtr]->SetEntry(key, value);
	}
	return false;
}

bool TIniProfile::SetInt(const char* key, int value)
{ // use SetString function to do this
	char buf[33];
	return SetString(key, itoa(value, buf, 10));
}

TIniProfile::TEntry* TIniProfile::GetEntry(unsigned p)
{ // check for sections
	if (SectionList.Count())
	{ // get section pointer
		TSection* section = SectionList[SectionListPtr];
		// check for valid index
		if (p < section->EntryList.Count())
		{ // return entry pointer
			return section->EntryList[p];
		}
	}
	// return entry pointer NULL if not exist
	return nullptr;
}

bool TIniProfile::InsertComment(const char* key, const char* comment)
{
	// Check section entry count.
	if (SectionList.Count())
	{
		// set dirty flag so the write function really writes to file.
		Dirty = true;
		// Look key up in current section and set the entry.
		return SectionList[SectionListPtr]->InsertComment(key, comment);
	}
	return false;
}

unsigned TIniProfile::FindSection(const char* section)
{
	if (!section)
	{
		return SectionListPtr;
	}
	// loop through section list
	for (unsigned i = 0; i < SectionList.Count(); i++)
	{  // section name is case insensitive compared
		if (!::strcmp(SectionList[i]->Name.c_str(), section))
		{ // return position
			return i;
		}
	}
	// return UINT_MAX on not found
	return UINT_MAX;
}

bool TIniProfile::RemoveSection(unsigned p)
{ // check for valid parameters
	if (p != UINT_MAX && p < SectionList.Count())
	{ // delete instance
		delete_null(SectionList[p]);
		// remove from list
		SectionList.Detach(p);
		// correct current section index pointer
		if (p < SectionListPtr)
		{
			SectionListPtr--;
		}
		if (SectionListPtr >= SectionList.Count())
		{
			SectionListPtr = SectionList.Count() - 1;
		}
		// set dirty flag
		Dirty = true;
		// return true on success
		return true;
	}
	return false;
}

//=============================================================================
// TSection Members
//=============================================================================
//
TIniProfile::TSection::~TSection()
{
	TEntryListIterator i(EntryList);
	while (i)
	{
		delete (i++);
	}
}

bool TIniProfile::TSection::Read(std::istream& is)
{
	// check stream for errors
	if (is.good())
	{  // find section begin, is the first '[' character on a line
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
			{  // create empty entry
				auto entry = new TEntry();
				// read entry from if stream has no errors and entry
				if (entry->Read(is) && entry->IsValid())
				{
					EntryList.Add(entry);
				}
				else
				{
					delete_null(entry);
				}
				// skip empty lines
				skip_empty_lines(is);
			}
			// read of TSection suceeded
			return true;
		}
	}
	// error reading TSection
	return false;
}

std::ostream& TIniProfile::TSection::Write(std::ostream& os) // NOLINT(readability-make-member-function-const)
{
	// check stream for errors
	if (os.good())
	{ // write section head
		os << '[' << Name << ']' << '\n';
		TEntryListIterator i(EntryList);
		// write all entries to the stream
		while (i)
		{
			(i++)->Write(os);
		}
	}
	return os;
}

const char* TIniProfile::TSection::GetEntry(const char* key, const char* defValue)
{
	unsigned p = FindEntry(key);
	return (p == UINT_MAX) ? defValue : EntryList[p]->GetValue();
}

bool TIniProfile::TSection::SetEntry(const char* key, const char* value)
{ // check for valid paramters
	if (key && value && strlen(key))
	{ // create the entry
		auto entry = new TEntry(key, value);
		// if not found p is equals UINT_MAX
		unsigned p = FindEntry(key);
		// if found
		if (p != UINT_MAX)
		{  // remove entry from heap
			delete_null(EntryList[p]);
			// assign new entry instance pointer to same location in list
			EntryList[p] = entry;
		}
		else // if not found
		{ // add entry at the end of the section list
			EntryList.Add(entry);
		}
		return true;
	}
	return false;
}

bool TIniProfile::TSection::InsertComment(const char* key, const char* comment)
{
	// check for valid parameters
	if (key && comment && strlen(key))
	{ // create the comment entry
		auto entry = new TEntry(comment);
		// if not found p is equals UINT_MAX
		unsigned p = FindEntry(key);
		// if found
		if (p != UINT_MAX)
		{  // add entry at found location
			EntryList.AddAt(entry, p);
			return true;
		}
	}
	// comment not inserted
	return false;
}

unsigned TIniProfile::TSection::FindEntry(const char* key)
{ // test if key is valid
	if (key)
	{ // loop through entry list
		for (unsigned p = 0; p < EntryList.Count(); p++)
		{  // section name is case insesitive compared
			if (!::strcmp(EntryList[p]->GetKey(), key))
			{
				// return pointer in list
				return p;
			}
		}
	}
	// return UINT_MAX when not found
	return UINT_MAX;
}

bool TIniProfile::TSection::RemoveEntry(unsigned p)
{ // check for valid parameters
	if (p != UINT_MAX && p < EntryList.Count())
	{  // delete entry instance
		delete_null(EntryList[p]);
		// remove from list
		EntryList.Detach(p);
		// return true on success
		return true;
	}
	return false;
}

//=============================================================================
// TEntry Members
//=============================================================================
//
TIniProfile::TEntry::TEntry(const char* key, const char* value)
	: Line(nullptr)
	, ValPos(0)
{
	Set(std::string(key).append("=").append(value));
}

TIniProfile::TEntry::TEntry(const char* comment)
	: Line(nullptr)
	, ValPos(0)
{
	// make comment line
	Set(std::string("; ").append(comment));
}

TIniProfile::TEntry::~TEntry()
{
	free_null(Line);
}

bool TIniProfile::TEntry::Set(const std::string& line)
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
		{  // save value position position
			size_t p = ValPos;
			// seperate key and value by placing a terminator
			Line[ValPos++] = 0;
			// skip white space after key part of std::string
			while (Line[--p] == ' ' && p)
			{
				Line[p] = 0;
			}
			// check for valid key
			if (!p)
			{  // free and clear because pointer is also flag
				free_null(Line);
			}
			else
			{  // skip white space infront of value
				while (Line[ValPos] == ' ')
				{
					Line[ValPos++] = 0;
				}
			}
		}
		else
		{
			// free and clear because pointer is also flag
			free_null(Line);
		}
	}
	else
	{
		// if it is a comment line set ValPos to zero
		ValPos = 0;
	}
	// if Line has a value the function succeeded
	return Line != nullptr;
}

bool TIniProfile::TEntry::Read(std::istream& is)
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

const char* TIniProfile::TEntry::GetKey()
{ // check if line is valid
	return (Line && ValPos) ? Line : "";
}

const char* TIniProfile::TEntry::GetValue()
{  // check if line is valid
	return Line ? &Line[ValPos] : "";
}

std::ostream& TIniProfile::TEntry::Write(std::ostream& os)
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
