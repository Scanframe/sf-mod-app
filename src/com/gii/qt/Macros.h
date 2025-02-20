#pragma once

#include <gii/qt/Namespace.h>
/**
 * @brief Declaration of ID handling functions for Information ID properties.
 * @see #SF_IMPL_INFO_ID
 */
#define SF_DECL_INFO_ID(Name)                                                                                                                                  \
	[[nodiscard]] gii::IdType get##Name() const;                                                                                                                 \
	void set##Name(gii::IdType id);

/**
 * @brief Implementations of ID handling functions for Information ID properties.
 * @see #SF_DECL_INFO_ID
 */
#define SF_IMPL_INFO_ID(Class, Name, Instance)                                                                                                                 \
	gii::IdType Class::get##Name() const                                                                                                                         \
	{                                                                                                                                                            \
		return (Instance).getDesiredId();                                                                                                                          \
	}                                                                                                                                                            \
                                                                                                                                                               \
	void Class::set##Name(gii::IdType id)                                                                                                                        \
	{                                                                                                                                                            \
		(Instance).setup(id, true);                                                                                                                                \
	}
