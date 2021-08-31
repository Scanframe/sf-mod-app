#pragma once

#include "ResultDataTypes.h"
#include <misc/gen/Range.h>

namespace sf
{

/**
 * @brief Class used to give a ResultData instance access to virtual method of a derived class.
 */
class _GII_CLASS ResultDataHandler :public ResultDataTypes
{
	public:
		/**
		 * @brief Pure virtual function which must be overloaded in a derived class.
		 */
		virtual void resultDataEventHandler
			(
				EEvent event,
				const ResultData& caller,
				ResultData& link,
				const Range& rng,
				bool same_inst
			) = 0;

		/**
		 * @brief Destructor
		 *
		 * Clears the link with result instances so no errors occur when the link is destructed before the result.
		 */
		~ResultDataHandler();

		friend class ResultDataTypes;

		friend class ResultData;
};

template<class T>
class TResultDataLinkHandler :public ResultDataHandler
{
	public:
		/**
		 * @brief Required event handler type.
		 */
		typedef void (T::*TPmf)
			(
				EEvent event,
				const ResultData& caller,
				ResultData& link,
				const Range& rng,
				bool same_inst
			);

		/**
		 * @brief One and only template constructor.
		 * @param _this
		 * @param pmf Pointer to member function.
		 */
		inline
		TResultDataLinkHandler(T* _this, TPmf pmf)
			:_self(_this), _pmf(pmf) {}

		/**
		 * @brief Prevent copying.
		 */
		TResultDataLinkHandler(const TResultDataLinkHandler&) = delete;

		/**
		 * @brief Prevent assignment.
		 */
		TResultDataLinkHandler& operator=(const ResultDataHandler&) = delete;

	private:
		/**
		 * @brief Pointer to member function.
		 */
		TPmf _pmf;
		/**
		 * @brief Void pointer to class instance.
		 */
		T* _self;

		/**
		 * Call the member function through virtual overloaded function from the base class.
		 */
		void resultDataEventHandler
			(
				EEvent event,
				const ResultData& caller,
				ResultData& link,
				const Range& rng,
				bool same_inst
			) override
		{
			(_self->*_pmf)(event, caller, link, rng, same_inst);
		}
};

}
