#include <ctime>

#include "target.h"
#include "gen_utils.h"
#include "Sustain.h"
#if IS_WIN
#include "dbgutils.h"
#include "../win/WinTimer.h"
#endif

#define MAX_TIMER_REENTRIES 10

namespace sf
{

SustainBase::PtrVector* SustainBase::_defaultVector = nullptr;

SustainBase::SustainBase(PtrVector* vector, int priority)
	:_list(vector)
	 , _priority(priority)
{ // Check if the vector is a valid pointer.
	if (!_list)
	{ // Check also if the default vector has been created.
		if (!_defaultVector)
		{ // It has not so we create it.
			_defaultVector = new PtrVector();
		}
		// Assign the vector data member with the static default vector pointer value.
		_list = _defaultVector;
	}
	// When the priority is not spTIMER enable it by default.
	if (_priority != spTimer)
	{
		enable();
	}
	// Order on the priority of the entry priority 0 is the highest.
	unsigned i = _list->count();
	// Find the insert position based on the entry priority.
	while (i)
	{
		if (_priority > (*_list)[i - 1]->_priority)
		{
			break;
		}
		i--;
	}
	// Add this entry to the passed vector at position 'i'.
	_list->addAt(this, i);
}

SustainBase::~SustainBase()
{ // Check if the vector still exists to detach from.
	if (_list)
	{
		_list->detach(this);
		// Cleanup the default vector if the vector is empty.
		if (_list == _defaultVector)
		{
			unsigned count = _defaultVector->count();
			if (!count)
			{
				delete_null(_defaultVector);
			}
		}
		_list = nullptr;
	}
}

void SustainBase::flushVector(PtrVector* vector)
{
	if (vector)
	{
		PtrVector::iter_type i(*vector);
		while (i)
		{
			(i++)->_list = nullptr;
		}
		vector->flush();
	}
}

void SustainBase::callSustain(SustainBase::PtrVector* vector)
{
	if (!vector)
	{
		vector = _defaultVector;
	}
	if (vector)
	{ // Get the clock for all sustain functions.
		clock_t time = clock();
		// Do not use iterator because sustain could affect the vector itself.
		unsigned i = 0;
		while (i < vector->count())
		{
			SustainBase* entry((*vector)[i++]);
			// Check if the sustain function is on a timer by checking
			// if the priority has the lowest priority which is spTIMER.
			//
			if (entry->_priority == spTimer)
			{
				if (!entry->_timer(time))
				{
					continue;
				}
			}
			// Check if entry was disabled. This may look strange but the timers
			// enable disable is also used for non timer entries.
			if (entry->_timer.IsEnabled())
			{ // IF the hooked function returns false disable this entry.
				if (!entry->call(time))
				{
					entry->disable();
				}
			}
		}
	}
}

#if IS_WIN

class SustainWinTimer :public WinTimer
{
	public:
		/**
		 * Constructor.
		 */
		SustainWinTimer() :WinTimer()
		{}
		/**
		 * Overloaded from base class.
		 */
		void TimerRoutine(DWORD ticks) override
		{
			(void) ticks;
			static int inprogress = 0;
			// Place sentry for this part of the code
			if (inprogress < MAX_TIMER_REENTRIES)
			{
				// Set sentry
				inprogress = true;
				// Iterate through all functions in the table.
				SustainBase::callSustain();
				// Make entry of the function possible.
				inprogress = false;
			}
			else
			{
				_NORM_NOTIFY(DO_DEFAULT, "SustainTimerProc(): Skipped: Maximum re-entries reached!");
			}
		}

} StaticSustainWinTimer;

#endif

bool setSustainTimer(unsigned msec)
{
#if IS_WIN
	return StaticSustainWinTimer.Set(msec);
#endif
	(void) msec;
	return false;
}

unsigned getSustainTimer()
{
#if IS_WIN
	return StaticSustainWinTimer.GetInterval();
#endif
	return 0;
}

}