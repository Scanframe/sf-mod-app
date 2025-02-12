namespace sf
{

template<typename T, typename P>
TClassRegistration<T, P>::TClassRegistration(entries_t& entries)
	: _entries(&entries)
{}

template<typename T, typename P>
TClassRegistration<T, P>::TClassRegistration(const TClassRegistration<T, P>& inst)
	: _entries(inst._entries)
{}

template<typename T, typename P>
TClassRegistration<T, P>::TClassRegistration(TClassRegistration<T, P>&& inst)
	: _entries(inst._entries)
{}

template<typename T, typename P>
size_t TClassRegistration<T, P>::size() const
{
	return _entries->size();
}

template<typename T, typename P>
size_t TClassRegistration<T, P>::registerClass(const char* name, const char* description, const typename TClassRegistration<T, P>::callback_t& callback)
{
	// Sanity check on existing entry.
	if (find(name))
	{
		std::cout << __FUNCTION__ << ": Entry with the name '" << name << "' is already registered!" << std::endl;
		typename entries_t::const_iterator begin = _entries->begin();
		return std::distance(begin, lookup(name));
		//throw Exception("%s: Entry with name '%s' is already registered!", __FUNCTION__, name);
	}
	// Index of insertion is the current size.
	auto index = _entries->size();
	// Add the entry at of the list.
	_entries->push_back(entry_t(name, description, callback));
	// Return the insertion index.
	return index;
}

template<typename T, typename P>
typename TClassRegistration<T, P>::entries_t::const_iterator TClassRegistration<T, P>::lookup(const std::string& name) const
{
	// Sanity check.
	if (!name.empty() && !_entries->empty())
	{
		// Iterate through the list.
		for (typename entries_t::const_iterator it = _entries->begin(); it != _entries->end(); ++it)
		{
			// Compare the name and return it when found.
			if (name.compare(it->_name) == 0)
			{
				return it;
			}
		}
	}
	// Signal not found.
	return _entries->end();
}

template<typename T, typename P>
size_t TClassRegistration<T, P>::indexOf(const std::string& name) const
{
	auto it = lookup(name);
	// When not found
	if (it == _entries->end())
	{
		return npos;
	}
	// Return the mount on instances between begin and the looked up one with the passed name.
	// Add one because zero means not found.
	return std::distance(const_cast<const entries_t*>(_entries)->begin(), it);
}

template<typename T, typename P>
const typename TClassRegistration<T, P>::entry_t* TClassRegistration<T, P>::find(const std::string& name) const
{
	auto it = lookup(name);
	// When the lookup points to the end the entry is not found.
	if (it == _entries->end())
	{
		return nullptr;
	}
	// Convert iterator to entry pointer. (calls a dereferenced operator first).
	return &(*it);
}

template<typename T, typename P>
T* TClassRegistration<T, P>::create(const std::string& name, const P& params) const
{
	auto entry = find(name);
	if (entry == nullptr)
	{
		return nullptr;
	}
	return const_cast<entry_t*>(entry)->_callback(params);
}

template<typename T, typename P>
T* TClassRegistration<T, P>::create(size_t index, const P& params) const
{
	return _entries->at(index)._callback(params);
}

template<typename T, typename P>
const char* TClassRegistration<T, P>::getName(size_t index) const
{
	return _entries->at(index)._name;
}

template<typename T, typename P>
typename TClassRegistration<T, P>::strings_t TClassRegistration<T, P>::getNames() const
{
	strings_t rv;
	if (_entries)
	{
		for (entry_t& i: *_entries)
		{
			rv.add(i._name);
		}
	}
	return rv;
}

template<typename T, typename P>
const char* TClassRegistration<T, P>::getDescription(size_t index) const
{
	return _entries->at(index)._description;
}

}// namespace sf
