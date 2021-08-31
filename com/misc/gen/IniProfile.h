#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "TVector.h"
#include "../global.h"
#include "gen_utils.h"

namespace sf
{

/**
* @brief Class for reading and writing ini-profiles.
*/
class _MISC_CLASS IniProfile
{
	protected:
		class Entry;

		class Section;

	public:
		/**
		 * @brief Internally used type.
		 */
		typedef TVector<Entry*> EntryVector;

		/**
		 * @brief Index not found return value.
		 */
		static constexpr size_t npos = EntryVector::npos;

		/**
		 * @brief Type
		 */
		typedef typename EntryVector::size_type size_type;

		/**
		 * @brief Default constructor for empty instance.
		 */
		IniProfile();

		/**
		 * @brief Initializing constructor.
		 *
		 * @param path Filepath to the ini-file.
		 */
		explicit IniProfile(const std::string& path);

		/**
		 * @brief Copying this class is not possible.
		 */
		IniProfile(const IniProfile&) = default;

		/**
		 * @brief Initializing constructor.
		 * If filename = NULL the startup path is selected.
		 */
		IniProfile(const std::string& section, const std::string& path);

		/**
		 * @brief Initializing constructor.
		 * Create by reading from std::istream current position
		 */
		explicit IniProfile(std::istream& is);

		/**
		 * @brief Virtual destructor writes to file path when dirty.
		 */
		virtual ~IniProfile();

		/**
		 * @brief Works the same as TProfile from OWL
		 */
		[[nodiscard]] int getInt(const std::string& key, int defaultInt = 0) const;

		/**
		* @brief Works the same as TProfile from OWL
		*/
		bool getString(const std::string& key, std::string& value, const std::string& defaultString = {}) const;

		/**
		 * @brief Flushes/Removes all sections from this instance.
		 */
		void flush();

		/**
		 * @brief Prepends prefix to key if prefix not null
		 */
		void setKeyPrefix(const std::string& prefix = {});

		/**
		 * @brief Gets a string class value.
		 */
		[[nodiscard]] std::string getString(const std::string& key, const std::string& defaultString = {}) const;

		/**
		 * @brief Sets a string class value making the Dirty flag true
		 */
		bool setString(const std::string& key, const std::string& st);

		/**
		 * @brief Sets an (int) value and sets the Dirty flag to true
		 */
		bool setInt(const std::string& key, int value);

		/**
		 * @brief Sets current section, returns true if section exist
		 * and creates one if 'create' is true.
		 */
		bool setSection(const std::string& section, bool create = true);

		/**
		 * Selects section without creating.
		 * @param section
		 * @return
		 */
		[[nodiscard]] bool selectSection(const std::string& section) const;

		/**
		 * @brief Sets current section by index.
		 *
		 * @param index Section index.
		 * @return true if section exist.
		 */
		bool setSection(size_type index);

		/**
		 * @brief Gets the section count.
		 *
		 * @return Amount of sections.
		 */
		[[nodiscard]] size_type getSectionCount() const;

		/**
		* @brief Get current or specific section name p is index in section list.
		*/
		[[nodiscard]] std::string getSection(size_type p = npos) const;

		/**
		* @brief Get all section names in a std::string vector.
		*/
		[[nodiscard]] strings getSections() const;

		/**
		 * @brief Finds current or specific section location in list.
		 * @return #npos on not found.
		 */
		[[nodiscard]] size_type findSection(const std::string& section) const;

		/**
		 * @brief Returns whether the passed section exist in the profile.
		 *
		 * @param section Section name.
		 * @return True when it exist.
		 */
		[[nodiscard]] bool sectionExists(const std::string& section) const
		{
			return findSection(section) != npos;
		}

		/**
		 * @brief Returns whether the passed key exist in the current selected in the section of the profile.
		 *
		 * @param key Key name.
		 * @return True when it exist.
		 */
		[[nodiscard]] bool keyExists(const std::string& key) const
		{
			// Lookup the key in current section and set the entry
			return _sections.empty() ? npos : _sections[_sectionIndex]->findEntry(key) != npos;
		}

		/**
		 * @brief Removes a section by name.
		 */
		bool removeSection(const std::string& section);

		/**
		 * @brief Removes a section by position.
		 */
		bool removeSection(size_type p);

		/**
		 * @brief Removes all keys from a section by position or the current selected one if none has been passed.
		 */
		bool removeKeys(size_type section = npos);

		/**
		 * @brief Inserts comment before existing key.
		 */
		bool insertComment(const std::string& key, const std::string& comment);

		/**
		 * @brief Returns amount of entries in the current section
		 */
		[[nodiscard]] size_type getEntryCount() const;

		/**
		 * @brief Finds in the current section the key, on failure it returns npos.
		 */
		size_type findEntry(const std::string& key);

		/**
		 * @brief Returns the entry value in current section, returns key
		 * in case of a comment line it returns a string of zero length
		 * p is index in entry (key) list (includes comments !!)
		 */
		std::string getEntryKey(size_type p);

		/**
		 * @brief Gets the keys of the current section or of the passed section index.

		 * @param section Section index where #npos indicates the current selected one.
		 * @return Vector of std::string of keys.
		 */
		[[nodiscard]] strings getKeys(size_type section = npos) const;

		/**
		 * @brief Type for retrieving key and values.
		 */
		typedef std::map<std::string, std::string> KeyValueMap;

		/**
		 * @brief Gets the key value map from the current selected section by default or the section provided.
		 *
		 * @param section Index of the section (default 'npos' = current)
		 * @return Map of keys and values.
		 */
		[[nodiscard]] KeyValueMap getMap(size_type section = npos) const;

		/**
		 * @brief Get entry key in current section, returns key or comment.
		 */
		std::string getEntryValue(size_type p);

		/**
		 * @brief Removes entry by key name.
		 */
		bool removeEntry(const std::string& key);

		/**
		 * @brief Removes entry or comment by position.
		 */
		bool removeEntry(size_type p);

		/**
		 * @brief Set the path of this instance.
		 *
		 * When the current file is dirty it is first written to disk.
		 * @param path Filepath to the ini-file.
		 * @return True on successful loading of the passed file.
		 */
		bool setFilepath(const std::string& path);

		/**
		 * Gets the file path when it is set.
		 * @return The ini filepath.
		 */
		[[nodiscard]] const std::string& getFilepath() const;

		/**
		 * @brief Set the write on dirty flag so changes are written on destruction.
		 * The default is true.
		 * @see #write()
		 * @param enable
		 */
		void setWriteOnDirty(bool enable)
		{
			_writeOnDirty = enable;
		}

		/**
		 * @brief Syncs the content to file when dirty.
		 *
		 * If the instance was created using a filepath the content is written when the dirty flag was set.
		 * @return True on success or when no filepath assigned.
		 */
		bool sync();

		/**
		 * @brief Write to std::ostream 'os' current position.
		 */
		virtual std::ostream& write(std::ostream& os) const;

		/**
		 * @brief Write current section to a 'ostream'.
		 */
		std::ostream& writeSection(std::ostream& os) const;

		/**
		 * @brief Tells if there write functions are used or not.
		 */
		[[nodiscard]] bool isDirty() const {return _dirty;}

		/**
		 * @brief Add sections from other std::istream.
		 */
		bool read(std::istream& is);

	protected:
		/**
		* @brief Function called in any constructor.
		*/
		void initialize();

		/**
		 * @brief InitializeBase from file path.
		 */
		bool load(const std::string& path);

		/**
		 * @brief InitializeBase from stream.
		 */
		bool load(std::istream& is);

		/**
		* @brief Internal storage class for keys.
		*/
		class _MISC_CLASS Entry
		{
			public:
				Entry() = default;

				Entry(const std::string& key, const std::string& value);

				explicit Entry(const std::string& comment);

				bool setLine(const std::string&);

				std::ostream& write(std::ostream& os);

				bool read(std::istream& is);

				[[nodiscard]] bool isValid() const;

			private:
				bool _cmtFlag{false};
				std::string _key;
				std::string _value;

				friend class IniProfile;
		};

		/**
		 * @brief Internal storage class for sections.
		 */
		class _MISC_CLASS Section
		{
			public:
				/**
				 * @brief Default constructor.
				 */
				Section()
					:_entries(0) {}

				/**
				 * @brief Destructor.
				 */
				~Section();

				/**
				 * @brief Finds an entry by key value.
				 *
				 * @param key Name of the key
				 * @return #npos if not found.
				 */
				IniProfile::EntryVector::size_type findEntry(const std::string& key);

				/**
				 * @brief Sets sections value by key name.
				 *
				 * @param key Name of the key
				 * @param value String value.
				 * @return -1 on failure and 1 on change and 0 on no change.
				 */
				int setEntry(const std::string& key, const std::string& value);

				/**
				 * @brief Gets an entry string and returns a default when it does not exist.
				 * @param key Key name.
				 *
				 * @param defValue Default string value.
				 * @return Found value or default when not found.
				 */
				std::string getEntry(const std::string& key, const std::string& defValue);

				/**
				 * @brief Removes an entry from a index position.
				 *
				 * @param index Position of the entry.
				 * @return True on success.
				 */
				bool removeEntry(EntryVector::size_type index);

				/**
				 * @brief Adds comment to before entry specified by the key.
				 *
				 * @param key Key name
				 * @param comment Comment added before the name entry.
				 * @return True when successful.
				 */
				bool insertComment(const std::string& key, const std::string& comment);

				/**
				 * @brief Streams section to an output stream.
				 *
				 * @param os Output stream.
				 * @return The passed output stream.
				 */
				std::ostream& write(std::ostream& os);

				/**
				 * @brief Read section from an input stream.
				 *
				 * @param is Input stream.
				 * @return True on success.
				 */
				bool read(std::istream& is);

				/**
				 * @brief Holds the name of the section
				 */
				std::string _name;

				/**
				 * @brief Holds the key value pairs.
				 */
				EntryVector _entries;
		};

		/**
		 * Internally used type.
		 */
		typedef TVector<Section*> SectionVector;

		/**
		 * Returns nullptr if not exist.
		 */
		Entry* getEntry(EntryVector::size_type p);

		/**
		 * Vector holding all sections.
		 */
		SectionVector _sections;
		/**
		 * current index to section.
		 */
		SectionVector::size_type _sectionIndex{npos};
		/**
		 * Path to file if it was created with one otherwise 'length' is zero
		 */
		std::string _path;
		/**
		 * Is Set to true if an entry has been changed.
		 */
		bool _dirty{false};
		/**
		 * If this flag is Set all fields read with 'ReadType' will be cleared
		 */
		bool _flagClearOnRead{true};
		/**
		 * When true the profile is written to the file path that was used to read it.
		 */
		bool _writeOnDirty{true};
		/**
		 * Holds the prefix for all entered 'key's'.
		 */
		std::string _prefix;
};

inline
IniProfile::size_type IniProfile::getSectionCount() const
{
	return _sections.count();
}

inline
bool IniProfile::Entry::isValid() const
{
	return !_key.empty();
}

inline
bool IniProfile::removeSection(const std::string& section)
{
	return removeSection(findSection(section));
}

inline
std::ostream& operator<<(std::ostream& os, const IniProfile& profile)
{
	return profile.write(os);
}

}
