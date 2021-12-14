#include "QtSync.h"

namespace sf
{

bool QtSync::Lock::lock(bool tryLock, int timeout)
{
	// Only when not locked.
	if (!_locked)
	{
		// When try locking.
		if (tryLock)
		{
			// When a timeout has been passed.
			if (timeout < 0)
			{
				_locked = _mutex.tryLock();
			}
			else
			{
				_locked = _mutex.tryLock(timeout);
			}
		}
		else
		{
			_mutex.lock();
			_locked = true;
		}
	}
	return _locked;
}

}
