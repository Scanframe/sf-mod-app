#pragma once

/**
 * @brief Declares unreferenced getter and unreferenced setter for a property.
 * @param Type Type of the property.
 * @param Name Suffix of the getXXX and setXXX methods being implemented.
 */
#define SF_DECL_PROP_GS(Type, Name) \
[[nodiscard]] Type get##Name() const; \
void set##Name(Type);

/**
 * @brief Declares unreferenced getter and referenced setter for a property.
 * @param Type Type of the property.
 * @param Name Suffix of the getXXX and setXXX methods being implemented.
 */
#define SF_DECL_PROP_GRS(Type, Name) \
[[nodiscard]] Type get##Name() const; \
void set##Name(const Type&);

/**
 * @brief Declares referenced getter and referenced setter for a property.
 * @param Type Type of the property.
 * @param Name Suffix of the getXXX and setXXX methods being implemented.
 */
#define SF_DECL_PROP_RGRS(Type, Name) \
[[nodiscard]] const Type& get##Name() const; \
void set##Name(const Type&);

/**
 * @brief Implements unreferenced getter.
 * @param Type Type of the property.
 * @param Class Class name to implement the method in.
 * @param Name Suffix of the getXXX method being implemented.
 * @param Instance The instance the getter is reading.
 */
#define SF_IMPL_PROP_G(Type, Class, Name, Instance) \
Type Class::get##Name() const \
{ \
  return Instance; \
}

/**
 * @brief Implements referenced getter.
 * @param Type Type of the property.
 * @param Class Class name to implement the methods in.
 * @param Name Suffix of the getXXX method being implemented.
 * @param Instance The instance the getter is reading.
 */
#define SF_IMPL_PROP_RG(Type, Class, Name, Instance) \
const Type& Class::get##Name() const \
{ \
  return Instance; \
}

/**
 * @brief Implements unreferenced setter.
 * @param Type Type of the property.
 * @param Class Class name to implement the methods in.
 * @param Name Suffix of the setXXX method being implemented.
 * @param Instance The instance the setter is writing.
 */
#define SF_IMPL_PROP_S(Type, Class, Name, Instance) \
void Class::set##Name(Type val) \
{ \
  Instance = val; \
}

/**
 * @brief Implements unreferenced setter with notification function.
 * @param Type Type of the property.
 * @param Class Class name to implement the methods in.
 * @param Name Suffix of the setXXX method being implemented.
 * @param Instance The instance the setter is writing.
 * @param NotifyFunc Must be of type void(void*)
 */
#define SF_IMPL_PROP_SN(Type, Class, Name, Instance, NotifyFunc) \
void Class::set##Name(Type val) \
{ \
  if (Instance != val) \
  { \
    Instance = val; \
    NotifyFunc(&Instance); \
  } \
}

/**
 * @brief Implements referenced setter.
 * @param Type Type of the property.
 * @param Class Class name to implement the methods in.
 * @param Name Suffix of the setXXX method being implemented.
 * @param Instance The instance the setter is writing.
 */
#define SF_IMPL_PROP_RS(Type, Class, Name, Instance) \
void Class::set##Name(const Type& val) \
{ \
 Instance = val; \
}

/**
 * @brief Implements referenced setter with notification function.
 * @param Type Type of the property.
 * @param Class Class name to implement the methods in.
 * @param Name Suffix of the setXXX method being implemented.
 * @param Instance The instance the setter is writing.
 * @param NotifyFunc Must be of type void(void*)
 */
#define SF_IMPL_PROP_RSN(Type, Class, Name, Instance, NotifyFunc) \
void Class::set##Name(const Type& val) \
{ \
  if (Instance != val) \
  { \
    Instance = val; \
    NotifyFunc(&Instance); \
  } \
}

/**
 * @brief Implements unreferenced setter and unreferenced getter.
 * @param Type Type of the property.
 * @param Class Class name to implement the methods in.
 * @param Name Suffix of the getXXX and setXXX methods being implemented.
 * @param Instance The instance the getter and setter is reading and writing.
 */
#define SF_IMPL_PROP_GS(Type, Class, Name, Instance) \
SF_IMPL_PROP_G(Type, Class, Name, Instance) \
SF_IMPL_PROP_S(Type, Class, Name, Instance)


#define SF_IMPL_PROP_GRS(Type, Class, Name, Instance) \
SF_IMPL_PROP_G(Type, Class, Name, Instance) \
SF_IMPL_PROP_RS(Type, Class, Name, Instance)


/**
 * @brief Implements unreferenced setter and unreferenced getter with notification function.
 * @param Type Type of the property.
 * @param Class Class name to implement the methods in.
 * @param Name Suffix of the getXXX and setXXX methods being implemented.
 * @param Instance The instance the getter and setter is reading and writing.
 * @param NotifyFunc Must be of type void(void*)
 */
#define SF_IMPL_PROP_GSN(Type, Class, Name, Instance, NotifyFunc) \
SF_IMPL_PROP_G(Type, Class, Name, Instance) \
SF_IMPL_PROP_SN(Type, Class, Name, Instance, NotifyFunc)

/**
 * @brief Implements referenced setter and unreferenced getter with notification function.
 * @param Type Type of the property.
 * @param Class Class name to implement the methods in.
 * @param Name Suffix of the getXXX and setXXX methods being implemented.
 * @param Instance The instance the getter and setter is reading and writing.
 * @param NotifyFunc Must be of type void(void*)
 */
#define SF_IMPL_PROP_GRSN(Type, Class, Name, Instance, NotifyFunc) \
SF_IMPL_PROP_G(Type, Class, Name, Instance) \
SF_IMPL_PROP_RSN(Type, Class, Name, Instance, NotifyFunc)

/**
 * @brief Implements referenced setter and referenced getter with notification function.
 * @param Type Type of the property.
 * @param Class Class name to implement the methods in.
 * @param Name Suffix of the getXXX and setXXX methods being implemented.
 * @param Instance The instance the getter and setter is reading and writing.
 * @param NotifyFunc Must be of type void(void*)
 */
#define SF_IMPL_PROP_RGRSN(Type, Class, Name, Instance, NotifyFunc) \
SF_IMPL_PROP_RG(Type, Class, Name, Instance) \
SF_IMPL_PROP_RSN(Type, Class, Name, Instance, NotifyFunc)
