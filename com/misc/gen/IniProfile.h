#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "TVector.h"

namespace sf
{

/**
* Class for reading and writing ini-profiles.
*/
class _MISC_CLASS IniProfile
{
	protected:
		/**
		* Internal storage class for keys.
		*/
		class Entry
		{
			public:
				Entry()
					:Line(nullptr)
					 , ValPos(0) {}

				Entry(const char* key, const char* value);

				explicit Entry(const char* comment);

				bool Set(const std::string&);

				~Entry();

				const char* GetKey();

				const char* GetValue();

				std::ostream& Write(std::ostream& os);

				bool Read(std::istream& is);

				[[nodiscard]] bool IsValid() const;

			private:
				size_t ValPos;
				char* Line;
		};

		/**
		* Internally used type.
		*/
		typedef TVector<Entry*> EntryListType;

		/**
		* Internal storage class for sections.
		*/
		class Section
		{
			public:

				Section()
					:EntryList(0) {}

				~Section();

				/**
				 * @param key Name of the key
				 * @return EntryListType::npos if not found.
				 */
				IniProfile::EntryListType::size_type FindEntry(const char* key);

				bool SetEntry(const char* key, const char* value);

				const char* GetEntry(const char* key, const char* defValue);

				bool RemoveEntry(EntryListType::size_type p);

				bool InsertComment(const char* key, const char* comment);

				std::ostream& Write(std::ostream& os);

				bool Read(std::istream& is);

				std::string Name;

				EntryListType EntryList;
		};

		/**
		 * Internally used type.
		 */
		typedef TVector<Section*> SectionListType;

		/**
		 * Returns nullptr if not exist.
		 */
		Entry* GetEntry(EntryListType::size_type p);

		/**
		 * Vector holding all sections.
		 */
		SectionListType SectionList;
		/**
		 * current index to section.
		 */
		EntryListType::size_type SectionListPtr;
		/**
		 * Path to file if it was created with one otherwise 'length' is zero
		 */
		std::string Path;
		/**
		 * Is Set to true if an entry has been changed.
		 */
		bool Dirty;
		/**
		 * If this flag is Set all fields read with 'ReadType' will be cleared
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

	public:

		static const size_t npos = EntryListType::npos;
		typedef typename EntryListType::size_type size_type;

		/**
		* Default constructor for empty instance
		*/
		IniProfile();

		/**
		* If filename = NULL the startup path is selected.
		*/
		explicit IniProfile(const char* path);

		/**
		* If filename = NULL the startup path is selected.
		*/
		IniProfile(const char* section, const char* path);

		/**
		* Create by reading from std::istream current position
		*/
		explicit IniProfile(std::istream& is);

		/**
		* Virtual destructor writes to file path when dirty.
		*/
		virtual ~IniProfile();

		/**
		* Works the same as TProfile from OWL
		*/
		int GetInt(const char* key, int defaultInt = 0) const;

		/**
		* Works the same as TProfile from OWL
		*/
		bool GetString(const char* key, char buff[], size_t buffSize,
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
		bool SetSection(size_type p);

		/**
		* Returns the count of sections.
		*/
		[[nodiscard]] size_type GetSectionCount() const;

		/**
		* Get current or specific section name p is index in section list.
		*/
		[[nodiscard]] const char* GetSection(size_type p = npos) const;

		/**
		* Finds current or specific section location in list.
		* Returns UINT_MAX on not found.
		*/
		size_type FindSection(const char* section = nullptr);

		/**
		* Removes a section by name.
		*/
		bool RemoveSection(const char* section);

		/**
		* Removes a section by position.
		*/
		bool RemoveSection(size_type p);

		/**
		* Inserts comment before existing key.
		*/
		bool InsertComment(const char* key, const char* comment);

		/**
		* Returns amount of entries in the current section
		*/
		[[nodiscard]] size_type GetEntryCount() const;

		/**
		* Finds in the current section the key, on failure it returns UINT_MAX.
		*/
		size_type FindEntry(const char* key);

		/**
		* Returns the entry value in current section, returns key
		* in case of a comment line it returns a string of zero length
		* p is index in entry (key) list (includes comments !!)
		*/
		const char* GetEntryKey(size_type p);

		/**
		* Get entry key in current section, returns key or comment.
		*/
		const char* GetEntryValue(size_type p);

		/**
		* Removes entry by key name.
		*/
		bool RemoveEntry(const char* key);

		/**
		* Removes entry or comment by position.
		*/
		bool RemoveEntry(size_type p);

		/**
		* Returns the file path if specific constructor was used for this instance.
		*/
		const char* GetFilePath()
		{
			return Path.c_str();
		}

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
		 * The default is true.
		 * @see #Write()
		 * @param enable
		 */
		void SetWriteOnDirty(bool enable)
		{
			WriteOnDirty = enable;
		}

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
		[[nodiscard]] bool IsDirty() const {return Dirty;}

		/**
		* Add sections from other std::istream.
		*/
		bool Read(std::istream& is);

	protected:
		/**
		* Protected function especially for derived classes.
		*/
		explicit IniProfile(bool skip_init);

		/**
		* Function called in any constructor.
		*/
		void Initialize();

		/**
		* InitializeBase from file path.
		*/
		bool Init(const char* path);

		/**
		* InitializeBase from stream.
		*/
		bool Init(std::istream& is);

};

inline
IniProfile::size_type IniProfile::GetSectionCount() const
{
	return SectionList.count();
}

inline
bool IniProfile::Entry::IsValid() const
{
	return Line != nullptr;
}

inline
bool IniProfile::WriteString(const char* key, const char* value)
{
	return SetString(key, value);
}

inline
bool IniProfile::WriteInt(const char* key, int value)
{
	return SetInt(key, value);
}

inline
IniProfile::size_type IniProfile::GetEntryCount() const
{
	return SectionList.count() ? SectionList[SectionListPtr]->EntryList.count() : 0;
}

inline
const char* IniProfile::GetEntryKey(IniProfile::size_type p)
{
	Entry* entry = GetEntry(p);
	return entry ? entry->GetKey() : nullptr;
}

inline
const char* IniProfile::GetEntryValue(IniProfile::size_type p)
{
	Entry* entry = GetEntry(p);
	return entry ? entry->GetValue() : nullptr;
}

inline
std::ostream& IniProfile::WriteSection(std::ostream& os) const
{
	// Check section entry count.
	return (SectionList.count()) ? SectionList[SectionListPtr]->Write(os) : os;
}

inline
bool IniProfile::RemoveSection(const char* section)
{
	return RemoveSection(FindSection(section));
}

inline
bool IniProfile::RemoveEntry(IniProfile::size_type p)
{
	// Set dirty flag
	Dirty = true;
	if (SectionList.count())
	{
		return SectionList[SectionListPtr]->RemoveEntry(p);
	}
	return false;
}

inline
bool IniProfile::RemoveEntry(const char* key)
{
	// Set dirty flag
	Dirty = true;
	if (SectionList.count())
	{
		return SectionList[SectionListPtr]->RemoveEntry(SectionList[SectionListPtr]->FindEntry(key));
	}
	return false;
}

inline
bool IniProfile::SetString(const char* key, const std::string& st)
{
	return SetString(key, st.c_str());
}

inline
std::ostream& operator<<(std::ostream& os, const IniProfile& profile)
{
	return profile.Write(os);
}

} // namespace
