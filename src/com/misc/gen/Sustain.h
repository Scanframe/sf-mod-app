#pragma once

#include "../global.h"
#include "IntervalTimer.h"
#include "TVector.h"
#include "target.h"

namespace sf
{

/*
 * @brief Base class for template `TSustain` which enables repetitive calls from the main thread with a set frequency.
 *
 * Purpose:
 *   Enables calling of sustain member functions of classes.
 *   The member functions must be of the type 'bool A_Name(int)'.
 *   This function is hooked into a table using class template 'TSustain'.
 *
 * Usage:
 *   // In class declaration.
 *   TSustain< cls > Entry;
 *   // Function must return true if it wants to be called again.
 *   bool call(const timespec& t);
 *
 *   // Line in constructor
 *   Entry(this, &cls::MySustainFunction, priority, nullptr)
 *
 *   // To set the freq of the timer, use
 *   Entry.setInterval(const timespec& interval);
 *
 *   // To enable all this, do once, at program startup...
 *   // (this timer determines the maximum freq of sustained functions
 *   setSustainTimer(unsigned int ms);
 */
class _MISC_CLASS SustainBase
{
	public:
		typedef TVector<SustainBase*> PtrVector;
		/**
		 * @brief Do not use an iterator because the sustain function could affect the vector itself.
		 */
		enum ESustainPriority : int
		{
			/** Default priority **/
			spDefault = 100,
			/** Maximum priority **/
			spTimer = std::numeric_limits<int>::max()
		};

		/**
		 * @brief Prevent copying.
		 */
		SustainBase(const SustainBase&) = delete;

		/**
		 * @brief Prevent copying.
		 */
		SustainBase& operator=(const SustainBase&) = delete;

	protected:
		/**
		 * @brief Default Constructor adding itself to the passed vector.
		 *
		 * If the passed vector is NULL the default vector is selected.
		 * @param vector
		 * @param priority
		 */
		SustainBase(PtrVector* vector, int priority);

		/**
		 * @brief oes not have to be virtual because this base class is not used must always be derived.
		 */
		~SustainBase();

	public:
		/**
		 * @brief Gets the priority value of this instance;
		 */
		[[nodiscard]] int getPriority() const { return _priority; }

		/**
		 * @brief Sets the interval at which the hooked function is called.
		 *
		 * This is only valid when the priority for this entry is #spTimer.
		 * @param interval In milliseconds.
		 */
		void setInterval(const timespec& interval)
		{
			_timer.set(interval);
		}

		/**
		 * @brief Gets the interval at which the hooked function is called.
		 *
		 * @return Interval in milliseconds.
		 */
		[[nodiscard]] const timespec& getInterval() const
		{
			return _timer.getInterval();
		}

		/**
		 * @brief Enables this entry.
		 */
		void enable()
		{
			_timer.enable();
		}

		/**
		 * @brief Disables this entry.
		 */
		void disable()
		{
			_timer.disable();
		}

		/**
		 * @brief  Returns if the entry is enabled or not.
		 *
		 * @return True when enabled.
		 */
		[[nodiscard]] bool isEnabled() const
		{
			return _timer.isEnabled();
		}

		/**
		 * @brief Must be overloaded to be able to call the sustain member function.
		 *
		 * @param time
		 * @return
		 */
		virtual inline bool call(const timespec& time)
		{
			return false;
		}

		/**
		 * @brief Flushes the vector and clears all Vector member of all entries first.
		 */
		static void flushVector(PtrVector* vector);

		/**
		 * @brief Calls all sustain table entry functions in the passed vector.
		 *
		 * The default is the static default vector.
		 * @param vector
		 */
		static void callSustain(PtrVector* vector = nullptr);

		/**
		 * @brief Pointer to the default vector which is automatically created when
		 */
		static PtrVector* _defaultVector;

	protected:
		/**
		 * @brief Data member holing priority.
		 */
		int _priority;
		/**
		 * @brief Vector where this entry is part of.
		 */
		PtrVector* _list;
		/**
		 * @brief Timer for when priority #spTimer has been set.
		 */
		IntervalTimer _timer;
};

/**
 * @brief Template to make the sustain system call a class method regularly.
 *
 * @tparam T Class containing the method.
 */
template<class T>
class TSustain : public SustainBase
{
	public:
		/**
		 * @brief Required type.
		 */
		typedef bool (T::*Pmf)(const timespec& t);

		/**
		 * @brief One and only initializing constructor.
		 *
		 * @param self This pointer.
		 * @param pmf Pointer to member function.
		 * @param priority The priority.
		 * @param vector Optional pointer to vector keeping the instance.
		 */
		TSustain(T* self, Pmf pmf, int priority = spDefault, PtrVector* vector = nullptr);

		/**
		 * @brief Prevent copying.
		 */
		TSustain(const TSustain&) = delete;

		/**
		 * @brief Prevent copying.
		 */
		TSustain& operator=(const TSustain&) = delete;

	private:
		/**
		 * @brief Pointer to member function.
		 */
		Pmf _pmf;
		/**
		 * @brief To class instance.
		 */
		T* _self;

		/**
		 * @brief Call the member function
		 *
		 * @param time Current clock time value.
		 * @return True when...
		 */
		inline bool call(const timespec& t) override
		{
			return (_self->*_pmf)(t);
		}
};

template<class T>
TSustain<T>::TSustain(T* self, Pmf pmf, int priority, PtrVector* vector)
	: SustainBase(vector, priority)
	, _self(self)
	, _pmf(pmf)
{
}

/**
 * @brief
 */
class _MISC_CLASS StaticSustain : public SustainBase
{
	public:
		/**
		 * @brief Required type for the callback function.
		 */
		typedef bool (*Pf)(const timespec&);

		/**
		 * @brief One and only constructor
		 *
		 * @param pf
		 * @param priority
		 * @param vector
		 */
		explicit StaticSustain(Pf pf, int priority = spDefault, PtrVector* vector = _defaultVector)
			: SustainBase(vector, priority)
			, _pf(pf)
		{}

		/**
		 * @brief Prevent copying.
		 */
		StaticSustain(const StaticSustain&) = delete;

		/**
		 * @brief Prevent copying.
		 */
		StaticSustain& operator=(const StaticSustain&) = delete;

	private:
		/**
		 * @brief Pointer to static function.
		 */
		Pf _pf{};

		/**
		 * @brief Call the assigned method.
		 */
		bool call(const timespec& time) override;
};

inline bool StaticSustain::call(const timespec& time)
{
	return _pf(time);
}

/**
 * @brief This function will enable a timer that call all Sustain functions in the
 * DefaultVector member of the 'TSustain' template class.
 *
 * Passing the repetition rate in milliseconds.
 * if the passed time is zero the timer stops.
 * Returns true if it was successful.
 * @param msec Time in milliseconds.
 */
_MISC_FUNC bool setSustainTimer(int msec);

/**
 * @brief Gets the current sustain timer interval in milli-seconds.
 *
 * @return Interval in milliseconds.
 */
_MISC_FUNC int getSustainTimer();

}// namespace sf