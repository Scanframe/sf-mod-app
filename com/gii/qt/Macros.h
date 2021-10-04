#pragma once

#include "Namespace.h"
/**
 * @brief Declaration of ID handling functions for Information ID properties.
 * @see #SF_IMPL_INFO_ID
 */
#define SF_DECL_INFO_ID(Name) \
[[nodiscard]] Gii::IdType get##Name() const; \
void set##Name(Gii::IdType id);

/**
 * @brief Implementations of ID handling functions for Information ID properties.
 * @see #SF_DECL_INFO_ID
 */
#define SF_IMPL_INFO_ID(Class, Name, Instance) \
Gii::IdType Class::get##Name() const \
{ \
  return (Instance).getDesiredId(); \
} \
 \
void Class::set##Name(Gii::IdType id) \
{ \
  (Instance).setup(id, true); \
}
