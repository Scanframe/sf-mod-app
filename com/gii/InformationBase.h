#pragma once

#include "misc/mcvector.h"
#include "misc/genutils.h"
#include "misc/mcvector.h"

#include "global.h"

namespace sf
{

/**
 * Base class for all generic information objects to be able to put them in a typed list together.
 */
class _GII_CLASS TInformationTypes
{
	public:
		/**
		 * Type used for the identifying integer.
		 */
		typedef uint64_t id_type;
		/**
		 * Type used for the flag integer.
		 */
		typedef int32_t flags_type;
};


/**
 * Base class for all generic information objects to be able to put them in a typed list together.
 */
class _GII_CLASS TInformationObject :public TInformationTypes
{
	public:
		/**
		 * Virtual destructor so derived classes can be destroyed by a pointer of this type.
		 */
		virtual ~TInformationObject() = default;

		typedef mcvector<id_type> TIdVector;
		typedef TIdVector::iter_type TIdIterator;
};

}