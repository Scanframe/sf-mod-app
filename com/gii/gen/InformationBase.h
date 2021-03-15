#pragma once

#include <misc/gen/TVector.h>
#include <misc/gen/gen_utils.h>

#include "../global.h"

namespace sf
{

/**
 * Base class for all generic information objects to be able to put them in a typed list together.
 */
class _GII_CLASS InformationTypes
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
		/**
		 * Type used for size of vectors.
		 */
		typedef size_t size_type;

		typedef TVector<id_type> IdVector;
};


/**
 * Base class for all generic information objects to be able to put them in a typed list together.
 */
class _GII_CLASS InformationBase
{
	public:
		/**
		 * Virtual destructor so derived classes can be destroyed by a pointer of this type.
		 */
		virtual ~InformationBase() = default;
};

}