#include "target.h"
#include "gen_utils.h"
#include "Sustain.h"
#if IS_QT
#include <QTimer>
#elif IS_WIN
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
		auto time = getTime(false);
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
			if (entry->_timer.isEnabled())
			{ // IF the hooked function returns false disable this entry.
				if (!entry->call(time))
				{
					entry->disable();
				}
			}
		}
	}
}

#if IS_QT

class SustainQtTimer
{
	public:
		SustainQtTimer()
		{
			QObject::connect(&_timer, &QTimer::timeout, [&]()
			{
				static int sentry = 0;
				// Place sentry for this part of the code
				if (sentry < MAX_TIMER_REENTRIES)
				{
					// Set sentry
					sentry++;
					// Iterate through all functions in the table.
					SustainBase::callSustain();
					// Make entry of the function possible.
					sentry--;
					// Prevent clang warning.
					(void) sentry;
				}
				else
				{
					SF_NORM_NOTIFY(DO_DEFAULT, "Skipped: Maximum re-entries reached!")
				}
			});
		}

		QTimer _timer;
};


#elif IS_WIN

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
				inprogress++;
				// Iterate through all functions in the table.
				SustainBase::callSustain();
				// Make entry of the function possible.
				inprogress--;
			}
			else
			{
				SF_NORM_NOTIFY(DO_DEFAULT, "SustainTimerProc(): Skipped: Maximum re-entries reached!");
			}
		}

};

#endif


#if IS_QT
SustainQtTimer* globalSustainQtTimer{nullptr};
#elif IS_WIN
SustainWinTimer* globalSustainWinTimer{nullptr};
#endif

bool setSustainTimer(int msec)
{
#if IS_QT
	if (!globalSustainQtTimer && msec > 0)
	{
		globalSustainQtTimer = new SustainQtTimer();
	}
	else
	{
		delete_null(globalSustainQtTimer);
	}
	if (globalSustainQtTimer)
	{
		globalSustainQtTimer->_timer.setInterval(msec);
		globalSustainQtTimer->_timer.start();
	}
	return true;
#elif IS_WIN
	if (!timer && msec > 0)
	{
		globalSustainWinTimer = new SustainWinTimer();
	}
	else
	{
		delete_null(globalSustainWinTimer);
	}
	if (globalSustainWinTimer)
	{
		return globalSustainWinTimer->set(msec);
	}
	return true;
#else
	(void) msec;
	return false;
#endif
}

int getSustainTimer()
{
#if IS_QT
	return globalSustainQtTimer->_timer.interval();
#elif IS_WIN
	return StaticSustainWinTimer.getInterval();
#endif
}

}