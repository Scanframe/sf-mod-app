/*
 * @brief
 * Purpose:
 *   Enables calling of sustain member functions of classes.
 *   The member functions must be of the type 'bool A_Name(int)'.
 *   This function is hooked into a table using class template 'TSustainTableEntry'.
 *
 * Usage:
 *   // In class declaration.
 *   TSustainTableEntry< cls > Entry;
 *   // Function must return true if it wants to be called again.
 *   bool Sustain(clock_t);
 *
 *   // Line in constructor
 *   Entry(this, &cls::MySustainFunction, priority, NULL)
 *
 *   // To set the freq of the timer, use
 *   Entry.SetInterval(int ms);
 *
 *   // To enable all this, do once, at program startup...
 *   // (this timer determines the maximum freq of sustained functions
 *   SetSustainTimer(unsigned int ms);
 */

#pragma once

#include "TVector.h"
#include "IntervalTimer.h"
#include "global.h"
#include "target.h"

namespace sf
{

class _MISC_CLASS SustainBase
{
	public:
		typedef TVector<SustainBase*> PtrVector;
		/**
		 * Do not use an iterator because the sustain function could affect the vector itself.
		 */
		enum ESustainPriority
		{
			spDefault = 100,
			spTimer = INT_MAX
		};

		/**
		 * Prevent copying.
		 */
		SustainBase(const SustainBase&) = delete;

		/**
		 * Prevent copying.
		 */
		SustainBase& operator=(const SustainBase&) = delete;

	protected:
		/**
		 * Default Constructor adding itself to the passed vector.
		 * If the passed vector is NULL the default vector is selected.
		 * @param vector
		 * @param priority
		 */
		SustainBase(PtrVector* vector, int priority);

		/**
		 * Does not have to be virtual because this base class is not used must always be derived.
		 */
		~SustainBase();

	public:
		/**
		 * Returns the priority value of this instance;
		 * @return
		 */
		[[nodiscard]] int getPriority() const {return _priority;}

		/**
		 * Sets the interval at which the hooked function is called.
		 * This is only valid when the priority for this entry is spTIMER.
		 * @param interval
		 */
		void setInterval(clock_t interval)
		{
			_timer.Set(interval);
		}

		/**
		 * Returns the interval at which the hooked function is called.
		 * @return Interval in msec.
		 */
		[[nodiscard]] clock_t getInterval() const
		{
			return _timer.GetIntervalTime();
		}

		/**
		 * Enables this entry.
		 */
		void enable()
		{
			_timer.Enable();
		}

		/**
		 * Disables this entry.
		 */
		void disable()
		{
			_timer.Disable();
		}

		/**
		 * Returns if the entry is enabled or not.
		 * @return
		 */
		[[nodiscard]] bool isEnabled() const
		{
			return _timer.IsEnabled();
		}

		/**
		 * Must be overloaded to be able to call the sustain member function.
		 * @param time
		 * @return
		 */
		virtual inline bool call(clock_t time)
		{
			return false;
		}

		/**
		 * Flushes the vector and clears all Vector member of all entries first.
		 */
		static void flushVector(PtrVector* vector);

		/**
		 * Calls all sustain table entry functions in the passed vector.
		 * The default is the static default vector.
		 * @param vector
		 */
		static void callSustain(PtrVector* vector = nullptr);

		/**
		 * Pointer to the default vector which is automatically created when
		 */
		static PtrVector* _defaultVector;

	protected:
		/**
		 * Member data
		 */
		int _priority;
		/**
		 * Vector where this entry is part of.
		 */
		PtrVector* _list;
		/**
		 * Timer for when priority spTIMER has been set.
		 */
		TIntervalTimer _timer;

	private:
		/**
		 * Type only for private use.
		 */
		typedef PtrVector::iter_type TPtrIterator;
};

template<class T>
class TSustain :public SustainBase
{
	public:
		/**
		 * Required type.
		 */
		typedef bool (T::*TPmf)(clock_t);

		/**
		 * One and only constructor
		 * @param self
		 * @param pmf
		 * @param priority
		 * @param vector
		 */
		TSustain(T* self, TPmf pmf, int priority = spDefault, PtrVector* vector = nullptr);

		/**
		 * Prevent copying.
		 */
		TSustain(const TSustain&) = delete;

		/**
		 * Prevent copying.
		 */
		TSustain& operator=(const TSustain&) = delete;

	private:
		/**
		 * void pointer to member function.
		 */
		TPmf _pmf;
		/**
		 * void pointer to class instance.
		 */
		T* _self;

		/**
		 * Call the member function
		 * @param time
		 * @return
		 */
		inline bool call(clock_t time) override
		{
			return (_self->*_pmf)(time);
		}
};

template<class T>
TSustain<T>::TSustain(T* self, TPmf pmf, int priority, PtrVector* vector)
	:SustainBase(vector, priority)
	 , _self(self)
	 , _pmf(pmf)
{
}

class _MISC_CLASS StaticSustain :public SustainBase
{
	public:
		/**
		 * Required type for the callback function.
		 */
		typedef bool (* TPf)(clock_t);

		/**
		 * One and only constructor
		 * @param pf
		 * @param priority
		 * @param vector
		 */
		explicit StaticSustain(TPf pf, int priority = spDefault, PtrVector* vector = _defaultVector)
			:SustainBase(vector, priority)
			 , _pf(pf) {}

		/**
		 * Prevent copying.
		 */
		StaticSustain(const StaticSustain&) = delete;

		/**
		 * Prevent copying.
		 */
		StaticSustain& operator=(const StaticSustain&) = delete;

	private:
		/**
		 * Void pointer to static function.
		 */
		TPf _pf{};

		/**
		 * Call the static function
		 * @param time
		 * @return
		 */
		inline bool call(clock_t time) override
		{
			return _pf(time);
		}
};

/**
 * This function will enable a timer that call all Sustain functions in the
 * DefaultVector member of the 'TSustainBaseTableEntry' class.
 * Passing the repetition rate in milliseconds.
 * if the passed time is zero the timer stops.
 * Returns true if it was successful.
 */
_MISC_FUNC bool setSustainTimer(unsigned msec);

/**
 * Returns the current sustain timer interval.
 */
_MISC_FUNC unsigned getSustainTimer();

}