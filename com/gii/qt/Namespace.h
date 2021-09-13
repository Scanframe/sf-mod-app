#pragma once

#include <QtCore/qglobal.h>
#include <QtCore/qtmetamacros.h>
#include <gen/InformationBase.h>
#include "../global.h"

namespace sf::Gii
{

Q_NAMESPACE_EXPORT(_GII_DATA)

/**
* @brief Short type definition for Qt code.
*/
typedef InformationTypes::id_type IdType;

/**
* @brief Types of id's.
*/
enum TypeId
{
	/** Type of the id is a variable. */
	Variable,
	/** Type of the id is a result-data. */
	ResultData
};
Q_ENUM_NS(TypeId)

/**
* @brief Mode of item selection.
*/
enum SelectionMode
{
	/** Single item. */
	Single,
	/** Multiple items. */
	Multiple
};
Q_ENUM_NS(SelectionMode)


/**
 * @brief Declaration of ID handling functions for Information ID properties.
 * @see #SF_IMPL_INFO_ID
 */
#define SF_DECL_INFO_ID(Name) \
[[nodiscard]] Gii::IdType getId##Name() const; \
void setId##Name(Gii::IdType id);

/**
 * @brief Implementations of ID handling functions for Information ID properties.
 * @see #SF_DECL_INFO_ID
 */
#define SF_IMPL_INFO_ID(Class, Name, Instance) \
Gii::IdType Class::getId##Name() const \
{ \
  return (Instance).getDesiredId(); \
} \
 \
void Class::setId##Name(Gii::IdType id) \
{ \
  (Instance).setup(id, true); \
}

}
