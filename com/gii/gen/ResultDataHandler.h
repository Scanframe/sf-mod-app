#pragma once

#include "ResultDataTypes.h"
#include <misc/gen/Range.h>

namespace sf
{

/**
 * @brief
 * Class used to give a TResultData instance access to virtual member function of a derived class.
 */
class ResultDataHandler :public ResultDataTypes
{
	public:
		/**
		 * Pure virtual function which must be overloaded in a derived class.
		 */
		virtual void ResultDataEventHandler
			(
				EEvent /*event*/,
				const ResultData& /*caller*/,
				ResultData& /*link*/,
				const Range& /*rng*/,
				bool /*same_inst*/
			) = 0;

		/**
		 * Destructor
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
		 * Required event handler type.
		 */
		typedef void (T::*TPmf)
			(
				EEvent event,
				const ResultData& caller,
				ResultData& link,
				const Range& rng,
				bool sameinst
			);

		/**
		 * One and only constructor
		 * @param _this
		 * @param pmf
		 */
		inline
		TResultDataLinkHandler(T* _this, TPmf pmf)
			:This(_this)
			 , Pmf(pmf) {}

		/**
		 * Prevent copying.
		 */
		TResultDataLinkHandler(const TResultDataLinkHandler&) = delete;

		/**
		 * Prevent assignment.
		 */
		TResultDataLinkHandler& operator=(const ResultDataHandler&) = delete;

	private:
		/**
		 * Pointer to member function.
		 */
		TPmf Pmf;
		/**
		 * Void pointer to class instance.
		 */
		T* This;

		/**
		 * Call the member function through virtual overloaded function from the base class.
		 */
		void ResultDataEventHandler
			(
				EEvent event,
				const ResultData& caller,
				ResultData& link,
				const Range& rng,
				bool sameinst
			) override
		{
			(This->*Pmf)(event, caller, link, rng, sameinst);
		}
};

}
