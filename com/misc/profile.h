/*
Class that can read\write windows like profile files.
*/

#ifndef MISC_PROFILE_H
#define MISC_PROFILE_H

#include <string>
#include <iostream>
#include <fstream>
#include <climits>

#include "mcvector.h"

namespace sf
{

/**
* Class for reading and writing ini-profiles.
*/
class _MISC_CLASS TIniProfile
{
	public:
		/**
		* Default constructor for empty instance
		*/
		TIniProfile();
		/**
		* If filename = NULL the startup path is selected.
		*/
		explicit TIniProfile(const char* path);
		/**
		* If filename = NULL the startup path is selected.
		*/
		TIniProfile(const char* section, const char* path);
		/**
		* Create by reading from std::istream current position
		*/
		explicit TIniProfile(std::istream& is);
		/**
		* Virtual destructor writes to file path when dirty.
		*/
		virtual ~TIniProfile();
		/**
		* Works the same as TProfile from OWL
		*/
		int GetInt(const char* key, int defaultInt = 0) const;
		/**
		* Works the same as TProfile from OWL
		*/
		bool GetString(const char* key, char buff[], unsigned buffSize,
			const char* defaultString = nullptr) const;
		/**
		* Works the same as TProfile from OWL
		*/
		bool WriteInt(const char* key, int value);
		/**
		* Works the same as TProfile from OWL
		*/
		bool WriteString(const char* key, const char* value);
		/**
		* Flushes/Removes all sections from this instance.
		*/
		void Flush();
		/**
		* Prepends prefix to key if prefix not null
		*/
		void SetKeyPrefix(const char* prefix = nullptr);
		/**
		* Gets a string class value.
		*/
		bool GetString(const char* key, std::string& value, const char* defaultString = nullptr) const;
		/**
		* Gets a string class value.
		*/
		std::string GetString(const char* key, const char* defaultString = nullptr) const;
		/**
		* Sets a string class value making the Dirty flag true
		*/
		bool SetString(const char* key, const std::string& st);
		/**
		* Sets a string value making the Dirty flag true
		*/
		bool SetString(const char* key, const char* st);
		/**
		* Sets an (int) value and sets the Dirty flag to true
		*/
		bool SetInt(const char* key, int value);
		/**
		* Sets current section, returns true if section exists
		* and creates one if 'create' is true.
		*/
		bool SetSection(const char* section = nullptr, bool create = true);
		/**
		* Sets current section, returns true if section exists.
		*/
		bool SetSection(unsigned p);
		/**
		* Returns the count of sections.
		*/
		unsigned GetSectionCount() const;
		/**
		* Get current or specific section name p is index in section list.
		*/
		const char* GetSection(unsigned p = UINT_MAX) const;
		/**
		* Finds current or specific section location in list.
		* Returns UINT_MAX on not found.
		*/
		unsigned FindSection(const char* section = nullptr);
		/**
		* Removes a section by name.
		*/
		bool RemoveSection(const char* section);
		/**
		* Removes a section by position.
		*/
		bool RemoveSection(unsigned p);
		/**
		* Inserts comment before existing key.
		*/
		bool InsertComment(const char* key, const char* comment);
		/**
		* Returns amount of entries in the current section
		*/
		unsigned GetEntryCount() const;
		/**
		* Finds in the current section the key, on failure it returns UINT_MAX.
		*/
		unsigned FindEntry(const char* key);
		/**
		* Returns the entry value in current section, returns key
		* in case of a comment line it returns a string of zero length
		* p is index in entry (key) list (includes comments !!)
		*/
		const char* GetEntryKey(unsigned p);
		/**
		* Get entry key in current section, returns key or comment.
		*/
		const char* GetEntryValue(unsigned p);
		/**
		* Removes entry by key name.
		*/
		bool RemoveEntry(const char* key);
		/**
		* Removes entry or comment by position.
		*/
		bool RemoveEntry(unsigned p);

		/**
		* Returns the file path if specific constructor was used for this instance.
		*/
		const char* GetFilePath() {return Path.c_str();}

		/**
		* Set the path of this instance.
		*/
		void SetFilePath(const char* path)
		{
			Flush();
			Init(path);
		}

		/**
		* Set the write on dirty flag so changes are written on destruction.
		* The default is true. @see #Write()
		*/
		void SetWriteOnDirty(bool enable) {WriteOnDirty = enable;}

		/**
		* If the instance was created with a file it is written to file.
		*/
		bool Write();
		/**
		* Write to std::ostreams 'os' current position.
		*/
		virtual std::ostream& Write(std::ostream& os) const;
		/**
		* Write current section to a 'ostream'.
		*/
		std::ostream& WriteSection(std::ostream& os) const;

		/**
		* Tells if there write functions are used or not.
		*/
		bool IsDirty() const {return Dirty;}

		/**
		* Add sections from other std::istream.
		*/
		bool Read(std::istream& is);

	protected:
		/**
		* Protected function especially for derived classes.
		*/
		explicit TIniProfile(bool skip_init);
		/**
		* Function called in any constructor.
		*/
		void Initialize();
		/**
		* Initialize from file path.
		*/
		bool Init(const char* path);
		/**
		* Initialize from stream.
		*/
		bool Init(std::istream& is);

		/**
		* Internal storage class for keys.
		*/
		class TEntry
		{
			public:
				TEntry()
				:Line(nullptr)
				,ValPos(0)
				{}

				TEntry(const char* key, const char* value);
				explicit TEntry(const char* comment);
				bool Set(const std::string&);
				~TEntry();
				const char* GetKey();
				const char* GetValue();
				std::ostream& Write(std::ostream& os);
				bool Read(std::istream& is);
				bool IsValid() const;

			private:
				size_t ValPos;
				char* Line;
		};

		/**
		* Internally used type.
		*/
		typedef mcvector<TEntry*> TEntryList;
		/**
		* Internally used type.
		*/
		typedef mciterator<TEntry*> TEntryListIterator;

		/**
		* Internal storage class for sections.
		*/
		class TSection
		{
			public:
				TSection()
					: EntryList(0) {}

				~TSection();
				// returns UINT_MAX if not found
				unsigned FindEntry(const char* key);
				bool SetEntry(const char* key, const char* value);
				const char* GetEntry(const char* key, const char* defValue);
				bool RemoveEntry(unsigned p);
				bool InsertComment(const char* key, const char* comment);
				std::ostream& Write(std::ostream& os);
				bool Read(std::istream& is);
				std::string Name;
				TEntryList EntryList;
		};

		/**
		* Internally used type.
		*/
		typedef mcvector<TSection*> TSectionList;
		/**
		* Internally used type.
		*/
		typedef mciterator<TSection*> TSectionListIterator;
		/**
		* Returns NULL if not exist.
		*/
		TEntry* GetEntry(unsigned p);
		/**
		* Vector holding all sections.
		*/
		TSectionList SectionList;
		/**
		* Current index to section.
		*/
		unsigned SectionListPtr;
		/**
		* Path to file if it was created with one otherwise 'length' is zero
		*/
		std::string Path;
		/**
		* Is set to true if an entry has been changed.
		*/
		bool Dirty;
		/**
		* If this flag is set all fields read with 'ReadType' will be cleared
		*/
		bool FlagClearOnRead;
		/**
		* When true the profile is written to the file path that was used to read it.
		*/
		bool WriteOnDirty;
		/**
		* Holds the prefix for all entered 'key's'.
		*/
		std::string Prefix;
};

inline
unsigned TIniProfile::GetSectionCount() const
{
	return SectionList.Count();
}

inline
bool TIniProfile::TEntry::IsValid() const
{
	return Line != nullptr;
}

inline
bool TIniProfile::WriteString(const char* key, const char* value)
{
	return SetString(key, value);
}

inline
bool TIniProfile::WriteInt(const char* key, int value)
{
	return SetInt(key, value);
}

inline
unsigned TIniProfile::GetEntryCount() const
{
	return SectionList.Count() ? SectionList[SectionListPtr]->EntryList.Count() : 0;
}

inline
const char* TIniProfile::GetEntryKey(unsigned p)
{
	TEntry* entry = GetEntry(p);
	return entry ? entry->GetKey() : nullptr;
}

inline
const char* TIniProfile::GetEntryValue(unsigned p)
{
	TEntry* entry = GetEntry(p);
	return entry ? entry->GetValue() : nullptr;
}

inline
std::ostream& TIniProfile::WriteSection(std::ostream& os) const
{
	// Check section entry count.
	return (SectionList.Count()) ? SectionList[SectionListPtr]->Write(os) : os;
}

inline
bool TIniProfile::RemoveSection(const char* section)
{
	return RemoveSection(FindSection(section));
}

inline
bool TIniProfile::RemoveEntry(unsigned p)
{  // set dirty flag
	Dirty = true;
	if (SectionList.Count())
	{
		return SectionList[SectionListPtr]->RemoveEntry(p);
	}
	return false;
}

inline
bool TIniProfile::RemoveEntry(const char* key)
{  // set dirty flag
	Dirty = true;
	if (SectionList.Count())
	{
		return SectionList[SectionListPtr]->RemoveEntry(SectionList[SectionListPtr]->FindEntry(key));
	}
	return false;
}

inline
bool TIniProfile::SetString(const char* key, const std::string& st)
{
	return SetString(key, st.c_str());
}

inline
std::ostream& operator<<(std::ostream& os, const TIniProfile& profile)
{
	return profile.Write(os);
}

} // namespace sf

#endif /* PROFILE_H_ */
