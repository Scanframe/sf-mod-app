/*
Class to register derived classes by a name so they can
be created using a static class member passing the name.
The advantage is that no header files have to be linked.

For this to work the base class must declare a class
called: Parameters and is used to pass parameters to constructors
as if each data member is a parameter.
A derived class must atleast have a constructor which passes
Parameters as a reference.

Defines to declare and implement registration for objects
	DECLARE_REGISTER_BASE(T)
	DECLARE_REGISTER_OBJECT(T)
	DECLARE_REGISTER_BASE_AND_OBJECT(T)

Implementation defines:
	IMPLEMENT_REGISTER_OBJECT(T, name, data)
	IMPLEMENT_REREGISTER_OBJECT(T, name, data)
	IMPLEMENT_REGISTER_BASE(T)
	IMPLEMENT_REGISTER_BASE_AND_OBJECT(T, name, data)

Static member functions added to the base class.
	static unsigned GetRegisterCount();
	static const char* GetRegisterName(unsigned i);
	static const void* GetRegisterData(unsigned i);
	static TRegObject* GetRegisterObject(unsigned i);
	static TRegObject* FindRegisterObject(const char* regname);
	static T* NewRegisterObject(unsigned i, T::Parameters& params);
	static T* NewRegisterObject(const char* regname, T::Parameters& params);

Member functions added to base class.
	const char* GetRegisterName() const;
	const void* GetRegisterData() const;
*/

#ifndef MISC_REGOBJ_H
#define MISC_REGOBJ_H

// Import of debugging derfines and macros.
#include "com/misc/dbgutils.h"
// Import of debugging derfines and macros.
#include "com/misc/genutils.h"

// Template class to use in a list
//
struct TRegObject
{ // Pointer to the static registered name.
	const char* Name;
	// Pointer to some extra information about the instance.
	const void* Data;
	// Pointer to the static function to create an instance of registerd class.
	void* (* NewObject)(const void*);
	// Some usefull types.
	typedef mcvector<TRegObject*> TPtrVector;
	typedef mciterator<TRegObject*> TPtrIterator;
};

//
// Function to find a registered class structure.
inline
TRegObject* FindRegObject(TRegObject::TPtrVector* list, const char* regname)
{
	if (list && regname && strlen(regname))
	{
		int count = list->Count();
		while (count--)
		{
			// Do a case insensitive compare.
			if (!strcmp((*list)[count]->Name, regname))
			{
				return (*list)[count];
			}
		}
	}
	return NULL;
}

//
// Function to find a registered class structure index.
inline
unsigned FindRegIndex(TRegObject::TPtrVector* list, const char* regname)
{
	if (list && regname && strlen(regname))
	{
		int count = list->Count();
		while (count--)
		{
			// Do a case insensitive compare.
			if (!strcmp((*list)[count]->Name, regname))
			{
				return count;
			}
		}
	}
	return UINT_MAX;
}

// Function to find and create registered class.
inline
void* NewRegObject(TRegObject::TPtrVector* list, const char* regname, const void* params)
{
	if (list)
	{
		TRegObject* regobj = FindRegObject(list, regname);
		if (regobj)
		{
			return regobj->NewObject(params);
		}
	}
	return NULL;
}

// Function to find and create registered class.
inline
void* NewRegObject(TRegObject::TPtrVector* list, unsigned index, const void* params)
{
	if (list)
	{
		// Check if index is in list range.
		TRegObject* regobj = (index < list->Count()) ? (*list)[index] : NULL;
		if (regobj)
		{
			return regobj->NewObject(params);
		}
	}
	return NULL;
}

// Template class to get a class registerd
template <class T>
class TObjectRegister : public TRegObject
{
	public:
		// Constructor
		TObjectRegister(const char* regname, const void* data = NULL)
			: RegList(T::RegObjectListPtr)
		{
			// Fill the registration structure.
			Name = regname;
			// Copy the additional dat structure.
			Data = data;
			// Assign the new object function for this type.
			NewObject = NewObjectFunc;
			// Create list if it is not there yet.
			// This is because there cannot be used a static list because
			// the linker decides what is created first.
			if (!RegList)
			{
				RegList = new TPtrVector();
			}
			// Add this structure to the list.
			RegList->Add(this);
			// Report the fact of registration.
			_NORM_NOTIFY(DO_DEFAULT, "TObjectRegister<T>: Registered '" << regname << '\'');
		}

		// Destructor
		~TObjectRegister()
		{ // Remove from registration list.
			RegList->Detach(this);
			// Check if the list is empty.
			if (!RegList->Count())
			{ // Delete the list and null the pointer.
				delete_null(RegList);
			}
		}

	private:
		// Holds the list where it is registered.
		TRegObject::TPtrVector*& RegList;
		static void* NewObjectFunc(const void* params);

};

//
template <class T>
void* TObjectRegister<T>::NewObjectFunc(const void* params)
{
	return new T(*(typename T::Parameters*) params);
}

//
// Macro used in other macros.
//
#define __DECLARE_REGOBJECTLISTPTR \
  static TRegObject::TPtrVector* RegObjectListPtr;

#define __DECLARE_REGOBJECTPTR \
  static TRegObject* RegObjectPtr;

#define __DECLARE_STATICREGISTERFUNCTIONS(T) \
  static unsigned GetRegisterCount() \
    { return RegObjectListPtr ? RegObjectListPtr->Count() : 0;} \
  static const char* GetRegisterName(unsigned i) \
    { return (RegObjectListPtr && i<RegObjectListPtr->Count()) ? (*RegObjectListPtr)[i]->Name : nullptr;} \
  static const void* GetRegisterData(unsigned i) \
    { return (RegObjectListPtr && i<RegObjectListPtr->Count()) ? (*RegObjectListPtr)[i]->Data : nullptr;} \
  static TRegObject* GetRegisterObject(unsigned i) \
    { return (RegObjectListPtr && i<RegObjectListPtr->Count()) ? (*RegObjectListPtr)[i] : nullptr;} \
  static TRegObject* FindRegisterObject(const char* regname) \
    {return ::FindRegObject(RegObjectListPtr, regname);} \
  static unsigned FindRegisterIndex(const char* regname) \
    {return ::FindRegIndex(RegObjectListPtr, regname);} \
  static T* NewRegisterObject(const char* regname, const T::Parameters& params) \
    {return (T*)::NewRegObject(RegObjectListPtr, regname, &params);} \
  static T* NewRegisterObject(unsigned i, const T::Parameters& params) \
    {return (T*)::NewRegObject(RegObjectListPtr, i, &params);} \
  const char* GetRegisterName() const \
    { return GetRegisterObject() ? GetRegisterObject()->Name : nullptr;} \
  const void* GetRegisterData() const \
    { return GetRegisterObject() ? GetRegisterObject()->Data : nullptr;} \

#define __DECLARE_REGISTER_FIEND(T) \
  friend class TObjectRegister<T>; \
  friend void __##T##RegisterOnLoad();

// Macro used to declare a registered base but the base is not registered object itself.
#define DECLARE_REGISTER_BASE(T) \
  protected: \
    __DECLARE_REGOBJECTLISTPTR \
  public: \
    __DECLARE_STATICREGISTERFUNCTIONS(T)\
    virtual const TRegObject* GetRegisterObject() const { return nullptr;}

// Macro used to declare a registered object of a class which is derived from registered base.
#define DECLARE_REGISTER_OBJECT(T) \
  public:\
    const TRegObject* GetRegisterObject() const override { return RegObjectPtr;}\
  protected: \
    __DECLARE_REGOBJECTPTR \
  __DECLARE_REGISTER_FIEND(T)

// Macro used to declare a registered base and the base is also a registered object itself.
#define DECLARE_REGISTER_BASE_AND_OBJECT(T) \
  public: \
    const TRegObject* GetRegisterObject() const override { return RegObjectPtr;} \
    __DECLARE_STATICREGISTERFUNCTIONS(T) \
  protected: \
    __DECLARE_REGOBJECTLISTPTR \
    __DECLARE_REGOBJECTPTR \
  __DECLARE_REGISTER_FIEND(T)

// Macro used to Implement a registered base but the base is not registered object itself.
#define IMPLEMENT_REGISTER_BASE(T)\
  TRegObject::TPtrVector* T::RegObjectListPtr = nullptr;

// Macro used to implement a registered object of a class which is derived from registered base.
#define IMPLEMENT_REGISTER_OBJECT_STATIC(T, name, data) \
  TObjectRegister<T> __##T##Register(name, data); \
  TRegObject* T::RegObjectPtr = &__##T##Register;

// Macro used to implement a registered object of a class which is derived from registered base.
#define IMPLEMENT_REGISTER_OBJECT(T, name, data)   \
  TRegObject* T::RegObjectPtr = nullptr; \
  __attribute__((constructor)) void __##T##RegisterOnLoad() \
  { \
    static TObjectRegister<T> __##T##Register(name, data);\
    T::RegObjectPtr = &__##T##Register; \
  }

 #define IMPLEMENT_REREGISTER_OBJECT(T, n, name, data) \
  __attribute__((constructor)) void __##T##Register##n##OnLoad() \
  { \
    static TObjectRegister<T> __##T##Register(name, data);\
  }

// Macro used to implement a registered base and the base is also a registered object itself.
#define IMPLEMENT_REGISTER_BASE_AND_OBJECT(T, name, data) \
  IMPLEMENT_REGISTER_BASE(T); \
  IMPLEMENT_REGISTER_OBJECT(T, name, data)

#endif // MISC_REGOBJ_H
