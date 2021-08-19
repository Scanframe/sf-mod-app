#include <string>
#include <cstring>

#include "gen_utils.h"
#include "IniProfile.h"

namespace sf
{

// Anonymous namespace.
namespace
{

std::istream& skip_empty_lines(std::istream& is)
{
	// now skip all new line character or carriage returns
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

}

void IniProfile::initialize()
{
	_sectionIndex = npos;
}

IniProfile::IniProfile()
{
	load("");
}

IniProfile::IniProfile(const std::string& path)
	:_path(path)
{
	load(path);
}

IniProfile::IniProfile(const std::string& section, const std::string& path)
{
	load(path);
	setSection(section);
}

IniProfile::IniProfile(std::istream& is)
{
	load(is);
}

IniProfile::~IniProfile()
{
	// Before destruction write all changes to file, if there is a change and it is allowed.
	if (_writeOnDirty)
	{
		sync();
	}
	// Clear all entries.
	flush();
}

void IniProfile::flush()
{
	// Delete all section instances.
	for (auto i: _sections)
	{
		delete i;
	}
	// Flush the pointers in the list.
	_sections.flush();
	// Reset the current index.
	_sectionIndex = npos;
}

bool IniProfile::load(const std::string& path)
{
	// create 'std::istream' using Path member variable
	if (sf::fileExists(path))
	{
		std::ifstream is(path.c_str(), std::ios::in/*, filebuf::openprot*/);
		load(is);
		return is.good();
	}
	return false;
}

bool IniProfile::load(std::istream& is)
{
	// Set dirty flag to false
	_dirty = false;
	// Read profile from stream
	bool rv = read(is);
	// Check size of profile and set current section.
	_sectionIndex = _sections.count() ? 0 : SectionVector::npos;
	return rv;
}

void IniProfile::setKeyPrefix(const std::string& prefix)
{
	_prefix = prefix;
}

bool IniProfile::read(std::istream& is)
{
	while (is.good())
	{
		// create empty section
		auto section = new Section;
		// read the next section
		if (section->read(is))
		{
			// add section on success
			_sections.add(section);
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

std::ostream& IniProfile::write(std::ostream& os) const
{
	if (os.good())
	{
		for (const auto& i: _sections)
		{
			i->write(os) << '\n';
		}
	}
	return os;
}

bool IniProfile::sync()
{
	// check dirty flag and Path member
	if (_dirty && !_path.empty())
	{
		// clear file upon opening
		std::ofstream os(_path.c_str(), std::ios::out | std::ios::trunc);
		// after file is written without errors Dirty flag is Set to false
		_dirty = !write(os).good();
		// write End Of File (EOF) character to the end
		//os->put('\x1A');
		// if not dirty file is written well or wasn't dirty
		return !_dirty;
	}
	// No path no failure to sync.
	return true;
}

std::string IniProfile::getSection(IniProfile::size_type p) const
{
	// return NULL if there are no section loaded at all
	if (!_sections.empty())
	{ // check if 'p' has a valid value
		if (p < _sections.count() || p == npos && _sectionIndex != npos)
		{
			return _sections.at((p == npos) ? _sectionIndex : p)->_name;
		}
	}
	return "";
}

strings IniProfile::getSections() const
{
	strings rv;
	for (auto& s: _sections)
	{
		rv.append(s->_name);
	}
	return rv;
}

bool IniProfile::setSection(IniProfile::size_type index)
{
	// Return NULL if there are no section loaded at all
	if (_sections.count())
	{
		// Check if 'index' has a valid value
		if (index < _sections.count())
		{
			_sectionIndex = index;
			return true;
		}
	}
	// On an invalid value keep current SectionListPtr
	return false;
}

bool IniProfile::setSection(const std::string& section, bool create)
{
	// Locate section
	_sectionIndex = findSection(section);
	// If found return true
	if (_sectionIndex != npos)
	{
		return true;
	}
	// Check if section must and can be created
	if (create && !section.empty())
	{
		// Create empty section
		auto s = new Section();
		// Assign name to it
		s->_name = section;
		// Add section to the list and make it the current one.
		_sectionIndex = _sections.add(s);
		// Set dirty flag
		_dirty = true;
		// Tells that section dit not exist at the start of this function
		return true;
	}
	// If section not found make first section the current one
	_sectionIndex = 0;
	// tells that section dit not exist at the start of this function
	return false;
}

bool IniProfile::selectSection(const std::string& section) const
{
	return const_cast<IniProfile*>(this)->setSection(section, false);
}

IniProfile::size_type IniProfile::findEntry(const std::string& key)
{
	auto rv = _sections.count() ? _sections[_sectionIndex]->findEntry(key) : npos;
	if (rv == npos)
	{
		if (_sections.count() && !key.empty())
		{
			std::clog << "IniProfile: Key '" << key << "' Not Found In Section '"
				<< _sections[_sectionIndex]->_name << "' In File '" << _path << std::endl;
		}
	}
	return rv;
}

bool IniProfile::getString(const std::string& key, std::string& value, const std::string& defaultString) const
{
	// Check if the section exists.
	if (_sectionIndex == npos)
	{
		value = defaultString;
		return false;
	}
	auto p = _sections[_sectionIndex]->findEntry(key);
	if (p == npos)
	{
		// if not found, copy default into the value and return false
		value = defaultString;
		// Signal failure.
		return false;
	}
	// Check for valid buffer
	value = _sections[_sectionIndex]->_entries[p]->_value;
	// Signal success.
	return true;
}

std::string IniProfile::getString(const std::string& key, const std::string& defaultString) const
{
	std::string value;
	getString(key, value, defaultString);
	return value;
}

int IniProfile::getInt(const std::string& key, int defaultInt) const
{
	// check section entries
	if (_sections.count())
	{
		auto value = _sections[_sectionIndex]->getEntry(key, "");
		return value.empty() ? defaultInt : std::stoi(value);
	}
	return defaultInt;
}

bool IniProfile::setString(const std::string& key, const std::string& value)
{
	// check section entry count
	if (_sections.count())
	{
		// Lookup the key in current section and set the entry
		auto rv = _sections[_sectionIndex]->setEntry(key, value);
		// Set dirty flag when a change occurred so the write function really writes to file
		_dirty |= rv > 0;
		return rv >= 0;
	}
	return false;
}

bool IniProfile::setInt(const std::string& key, int value)
{
	// Use SetString function to do this
	char buf[33];
	return setString(key, sf::itoa<int>(value, buf, 10));
}

IniProfile::Entry* IniProfile::getEntry(IniProfile::size_type p)
{
	// Check for sections
	if (_sections.count())
	{
		// Get section pointer
		Section* section = _sections[_sectionIndex];
		// Check for valid index
		if (p < section->_entries.count())
		{ // Return entry pointer
			return section->_entries[p];
		}
	}
	// return entry pointer NULL if not exist
	return nullptr;
}

bool IniProfile::insertComment(const std::string& key, const std::string& comment)
{
	// Check section entry count.
	if (_sections.count())
	{
		// Set dirty flag so the write function really writes to file.
		_dirty = true;
		// Look key up in current section and Set the entry.
		return _sections[_sectionIndex]->insertComment(key, comment);
	}
	return false;
}

IniProfile::size_type IniProfile::findSection(const std::string& section) const
{
	if (section.empty())
	{
		return _sectionIndex;
	}
	// Loop through section list
	for (IniProfile::size_type i = 0; i < _sections.count(); i++)
	{
		// Section name is case insensitive compared
		if (!::strcmp(_sections[i]->_name.c_str(), section.c_str()))
		{
			// Return position
			return i;
		}
	}
	// Return UINT_MAX on not found
	return npos;
}

bool IniProfile::removeSection(IniProfile::size_type p)
{
	// check for valid parameters
	if (p != npos && p < _sections.count())
	{
		// delete instance
		delete_null(_sections[p]);
		// remove from list
		_sections.detachAt(p);
		// correct current section index pointer
		if (p < _sectionIndex)
		{
			_sectionIndex--;
		}
		if (_sectionIndex >= _sections.count())
		{
			_sectionIndex = _sections.count() - 1;
		}
		// Set dirty flag
		_dirty = true;
		// return true on success
		return true;
	}
	return false;
}

bool IniProfile::removeKeys(IniProfile::size_type section)
{
	// Check if current must be selected.
	if (section == npos)
	{
		section = _sectionIndex;
	}
	// check for valid parameters
	if (section < _sections.count())
	{
		// delete instance
		for (auto e: _sections[section]->_entries)
		{
			delete e;
		}
		_sections[section]->_entries.clear();
		// Set dirty flag
		_dirty = true;
		// Signal success.
		return true;
	}
	return false;
}

IniProfile::Section::~Section()
{
	EntryVector::iter_type i(_entries);
	while (i)
	{
		delete (i++);
	}
}

bool IniProfile::Section::read(std::istream& is)
{
	// check stream for errors
	if (is.good())
	{
		// find section begin, is the first '[' character on a line
		if (is.get() != '[')
		{
			is.ignore(std::numeric_limits<std::streamsize>::max(), '[');
		}
		// test stream for errors
		if (is.good())
		{
			read_to_delim(_name, is, ']');
			// skip to end of this line
			skip_to_nextline(is);
			// check for section start character
			while (is.good() && is.peek() != '[')
			{
				// create empty entry
				auto entry = new Entry();
				// read entry from if stream has no errors and entry
				if (entry->read(is) && entry->isValid())
				{
					_entries.add(entry);
				}
				else
				{
					delete_null(entry);
				}
				// skip empty lines
				skip_empty_lines(is);
			}
			// Read of Section succeeded.
			return true;
		}
	}
	// error reading Section
	return false;
}

std::ostream& IniProfile::Section::write(std::ostream& os) // NOLINT(readability-make-member-function-const)
{
	// Check stream for errors
	if (os.good())
	{
		// Write section head
		os << '[' << _name << ']' << '\n';
		EntryVector::iter_type i(_entries);
		// write all entries to the stream
		while (i)
		{
			(i++)->write(os);
		}
	}
	return os;
}

std::string IniProfile::Section::getEntry(const std::string& key, const std::string& defValue)
{
	auto p = findEntry(key);
	return (p == npos) ? defValue : _entries[p]->_value;
}

int IniProfile::Section::setEntry(const std::string& key, const std::string& value)
{
	// Check for valid parameters.
	if (!key.empty())
	{
		// When not found p is equals npos.
		auto p = findEntry(key);
		// if entry was found
		if (p != npos)
		{
			if (_entries[p]->_value == value)
			{
				// Signal no change made.
				return 0;
			}
			else
			{
				// Remove entry from heap
				delete _entries[p];
				// Assign new entry instance pointer to same location in list.
				_entries[p] = new Entry(key, value);
				// Signal change.
				return 1;
			}
		}
			// if not found
		else
		{
			// Add entry at the end of the section list
			_entries.add(new Entry(key, value));
			// Signal change.
			return 1;
		}
	}
	// Signal failure.
	return -1;
}

bool IniProfile::Section::insertComment(const std::string& key, const std::string& comment)
{
	// Check for valid parameters
	if (!comment.empty() && !key.empty())
	{
		// Create the comment entry
		auto entry = new Entry(comment);
		// If not found p is equals npos.
		auto p = findEntry(key);
		// When found
		if (p != npos)
		{
			// Add entry at found location
			return _entries.addAt(entry, p);
		}
	}
	// Comment was not inserted
	return false;
}

IniProfile::EntryVector::size_type IniProfile::Section::findEntry(const std::string& key)
{
	// Check if key is valid.
	if (!key.empty())
	{
		EntryVector::size_type idx = 0;
		// Loop through entry list
		for (auto e: _entries)
		{
			// Section name is case-sensitive compared.
			if (e->_key == key)
			{
				// Return index in list
				return idx;
			}
			idx++;
		}
	}
	// Signal not found.
	return npos;
}

bool IniProfile::Section::removeEntry(EntryVector::size_type index)
{ // check for valid parameters
	if (index != npos && index < _entries.count())
	{
		// delete entry instance
		delete_null(_entries[index]);
		// remove from list
		_entries.detachAt(index);
		// return true on success
		return true;
	}
	return false;
}

IniProfile::Entry::Entry(const std::string& key, const std::string& value)
{
	setLine(std::string(key).append("=").append(value));
}

IniProfile::Entry::Entry(const std::string& comment)
{
	_cmtFlag = true;
	_value = comment;
}

bool IniProfile::Entry::setLine(const std::string& line)
{
	// Check if nothing to do.
	if (!line.empty())
	{
		// Check if this line is NOT a comment line by checking the first character.
		if (line.at(0) != ';' && line.at(0) != '#')
		{
			// Get key value separator position in the line.
			auto pos = line.find_first_of('=');
			_cmtFlag = false;
			// If not found the line is the keyword.
			if (pos == std::string::npos)
			{
				_key = line;
			}
			else
			{
				_key = line.substr(0, pos);
				_value = line.substr(pos + 1, line.length());
			}
		}
			// Comment line.
		else
		{
			_cmtFlag = true;
			// Strip the comment.
			_value = line.substr(1, line.length());
		}
	}
	return isValid();
}

bool IniProfile::Entry::read(std::istream& is)
{
	std::string line;
	getline(is, line);
	// When the end of the stream has been reached a string is not empty signal succes.
	if (line.length() && is.eof())
	{
		return setLine(line);
	}
	return is.good() && setLine(line);
}

IniProfile::size_type IniProfile::getEntryCount() const
{
	return _sections.count() ? _sections[_sectionIndex]->_entries.count() : 0;
}

std::string IniProfile::getEntryKey(IniProfile::size_type p)
{
	Entry* entry = getEntry(p);
	return entry ? entry->_key : std::string();
}

strings IniProfile::getKeys(size_type section) const
{
	strings rv;
	// Check if the current section is targeted.
	if (section == npos)
	{
		// Assign the current in section index to be used.
		section = _sectionIndex;
	}
	// Check if 'index' has a valid value
	if (section < _sections.count())
	{
		// Iterate through the sections entries.
		for (auto e: _sections.at(_sectionIndex)->_entries)
		{
			// Ignore comment lines.
			if (!e->_cmtFlag)
			{
				// Append the
				rv.append(e->_key);
			}
		}
	}
	return rv;
}

std::string IniProfile::getEntryValue(IniProfile::size_type p)
{
	Entry* entry = getEntry(p);
	return entry ? entry->_value : std::string();
}

std::ostream& IniProfile::writeSection(std::ostream& os) const
{
	// Check section entry count.
	return (_sections.count()) ? _sections[_sectionIndex]->write(os) : os;
}

bool IniProfile::removeEntry(IniProfile::size_type p)
{
	// Set dirty flag
	_dirty = true;
	if (_sections.count())
	{
		return _sections[_sectionIndex]->removeEntry(p);
	}
	return false;
}

bool IniProfile::removeEntry(const std::string& key)
{
	// Set dirty flag
	_dirty = true;
	if (_sections.count())
	{
		return _sections[_sectionIndex]->removeEntry(_sections[_sectionIndex]->findEntry(key));
	}
	return false;
}

bool IniProfile::setFilepath(const std::string& path)
{
	// Flush current entries.
	flush();
	// Assign the new file path.
	_path = path;
	// Load the file when it exists
	return load(path);
}

const std::string& IniProfile::getFilepath() const
{
	return _path;
}

IniProfile::KeyValueMap IniProfile::getMap(IniProfile::size_type section) const
{
	IniProfile::KeyValueMap rv;
	// Check if the current section is targeted.
	if (section == npos)
	{
		// Assign the current in section index to be used.
		section = _sectionIndex;
	}
	// Check if 'index' has a valid value
	if (section < _sections.count())
	{
		// Iterate through the sections entries.
		for (auto e: _sections.at(section)->_entries)
		{
			// Ignore comment lines.
			if (!e->_cmtFlag)
			{
				// Append the
				rv.insert(std::pair(e->_key, e->_value));
			}
		}
	}
	return rv;
}

std::ostream& IniProfile::Entry::write(std::ostream& os)
{
	// Check stream for errors
	if (os.good())
	{
		if (_cmtFlag)
		{
			os << ';' << _value << std::endl;
		}
		else if (_value.empty())
		{
			os << _key << std::endl;
		}
		else
		{
			os << _key << '=' << _value << std::endl;
		}
	}
	return os;
}

}
