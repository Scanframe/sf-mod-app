#include <misc/gen/dbgutils.h>
#include "ResultData.h"
#include "ResultDataHandler.h"
#include "ResultDataRequester.h"

namespace sf
{

/**
 * Sustain timer loop time for checking on possible errors.
 */
#define SUSTAIN_TIME 1000
/**
 * Default time for state machine to wait for an state change.
 */
#define DEF_TIMEOUT_TIME 30000

ResultDataRequester::ResultDataRequester()
	:_sustain(this, &ResultDataRequester::sustain, _sustain.spTimer)
	 , _dataHandler(this, &ResultDataRequester::resultCallback)
	 , _timeoutTimer(DEF_TIMEOUT_TIME)
	 , _handler(nullptr)
	 , _rdIndex(nullptr)
	// Is used for debugging timing of the state machine.
	 , _byPass(false)
{
	// Timer is just needed for testing the timeout.
	_sustain.setInterval(SUSTAIN_TIME);
}

ResultDataRequester::~ResultDataRequester()
{
	if (_rdIndex)
	{
		_rdIndex->setHandler(nullptr);
	}
}

void ResultDataRequester::reset()
{
	// Set the state of the machine.
	setState(drsReady);
	// Terminate the current work.
	_work.clear();
	// Disable the timer.
	_timeoutTimer.disable();
	//
	_rdIndex->clearRequests();
	//
	for (unsigned i = 0; i < _rdDataList.count(); i++)
	{
		_rdDataList[i]->clearRequests();
	}
}

void ResultDataRequester::SetLink(ResultDataHandler* link)
{
	_handler = link;
}

void ResultDataRequester::attachIndex(ResultData* rd)
{
	// Check a difference.
	if (_rdIndex != rd)
	{
		// Detach handler from current instance.
		if (_rdIndex)
		{
			_rdIndex->setHandler(nullptr);
			_rdIndex = rd;
		}
		// Assign the new result pointer.
		_rdIndex = rd;
		// When not zero for detaching link the handler to it.
		if (_rdIndex)
		{
			// Check if the result is already hooked to a handler.
			SF_COND_RTTI_NOTIFY(rd->getHandler(), DO_DEFAULT, "Index result is already linked to a handler!")
			// Link the handler.
			rd->setHandler(&_dataHandler);
		}
		// Reset the state machine when a change was eminent.
		reset();
	}
}

void ResultDataRequester::attachData(ResultData* rd)
{
	if (rd)
	{
		// Remove
		if (_rdDataList.find(rd) != UINT_MAX)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Tried to assign data result twice!")
			return;
		}
		// Add to the list of data results.
		_rdDataList.add(rd);
		//
		rd->setHandler(&_dataHandler);
		// Reset the state machine when a change was eminent.
		reset();
		//
		for (unsigned i = 0; i < _rdDataList.count(); i++)
		{
			// Assign the index of the result list in the results data field.
			_rdDataList[i]->setData(i);
		}
	}
}

void ResultDataRequester::detachData(ResultData* rd)
{
	// Find the result in the list.
	unsigned idx = _rdDataList.find(rd);
	if (idx != UINT_MAX)
	{
		// Check if the handler was reassigned.
		SF_COND_RTTI_NOTIFY(rd->getHandler() != &_dataHandler, DO_DEFAULT, "Linked handler was reassigned!")
		// Unlink the the handler.
		rd->setHandler(nullptr);
		// Remove result pointer from the list.
		_rdDataList.detachAt(idx);
		// Reset the state machine when a change was eminent.
		reset();
	}
}

void ResultDataRequester::resultCallback
	(
		ResultDataTypes::EEvent event,
		const ResultData& caller,
		ResultData& link,
		const Range& rng,
		bool same_inst
	)
{
	// Disable global events to pass beyond here.
	if (event < reFirstLocal)
	{
		return;
	}
	// Used for by passing data requests for debugging purposes.
	if (!_byPass)
	{
		// Handle the requester events.
		switch (event)
		{
			case ResultData::reIdChanged:
			case ResultData::reClear:
			{
				// When an id changes the state machine must be reset.
				reset();
			}
				break;

			case ResultData::reAccessChange:
				if (&link != _rdIndex)
				{
					if (!_work._dataAccess.Bits || !link.getId())
					{
						break;
					}
					// Check if this result has to catch up.
					if (_work._dataAccess.Has(link.getData()))
					{ // Check if the result has caught up with the working range of the index.
						if (rng.isWithinOther(_work._range))
						{
							_work._dataAccess.Unset(link.getData());
						}
						// Speed up thing by calling process when waiting is over.
						if (!_work._dataAccess.Bits)
						{
							process();
						}
					}
				}
				break;

			case ResultData::reGotRange:
				if (&link == _rdIndex)
				{
					if (!_work._indexRequest)
					{
						setError("Index GOTRANGE event not expected");
					}
					// Reset the flag.
					_work._indexRequest = false;
					// Speed up thing by calling process.
					process();
				}
				else
				{
					if (!_work._dataRequest.Has(link.getData()))
					{
						setError("Data GOTRANGE event not expected");
					}
					// Clear the bit of the request placed.
					_work._dataRequest.Unset(link.getData());
					// Speed up thing by calling process wen waiting is over.
					if (!_work._dataRequest.Bits)
					{
						process();
					}
					break;
				}

			default:
				break;
		}
	}
	// Emit the event.
	if (_handler)
	{
		_handler->handleResultDataEvent(event, caller, link, rng, same_inst);
	}
}

void ResultDataRequester::passIndexEvent(ResultDataRequester::EReqEvent event)
{
	if (_handler)
	{
		_handler->handleResultDataEvent
			(
				(ResultData::EEvent) event,
				*_rdIndex,
				*_rdIndex,
				_rdIndex->getId() ? _work._index : _work._range,
				true
			);
	}
}

bool ResultDataRequester::sustain(clock_t)
{
	// Processes the current state when not waiting for an event.
	if (_state != drsReady)
	{
		process();
	}
	// Want to be called next time too.
	return true;
}

const char* ResultDataRequester::getStateName(int state)
{
	switch (state)
	{
		case drsError:
			return "ERROR";
		case drsReady:
			return "READY";
		case drsGetIndex:
			return "GETINDEX";
		case drsReadIndex:
			return "READINDEX";
		case drsGetData:
			return "GETDATA";
		case drsApply:
			return "APPLY";
		case drsTryData:
			return "TRYDATA";
		case drsWait:
			return "WAIT";
		default:
			return "Unknown";
	}
}

bool ResultDataRequester::setState(ResultDataRequester::EState state)
{
	// Only change the previous state when the state is not psWAIT.
	if (_state != drsWait)
	{
		_previousState = _state;
	}
	// Assign the new state value.
	_state = state;
	// No waiting when state was set with this function.
	_waitForState = drsReady;
	//
	return true;
}

bool ResultDataRequester::setError(const std::string& txt)
{
	auto old_prev = _previousState;
	// Update the previous state.
	if (_state != drsWait)
	{
		_previousState = _state;
	}
	// Assign the new state first so that msg boxes can appear.
	_state = drsError;
	// Do some debug printing in case of an error.
	SF_RTTI_NOTIFY(DO_DEFAULT, "State Machine ran into an error '"
		<< txt << "'. SetState(" << getStateName(old_prev) << "=>"
		<< getStateName(_previousState) << "=>" << getStateName(_state)	<< ")" << _work._index << _work._range << '!')
	//
	SF_RTTI_NOTIFY(DO_DEFAULT, "Resetting the state machine.")
	// Reset the state machine after each error.
	reset();
	// Always return false for being able to use it as a return value.
	return false;
}

bool ResultDataRequester::waitForState(ResultDataRequester::EState state)
{
	//RTTI_NOTIFY(DO_DEFAULT, "WaitForState(" << GetStateName(state) << ")" << FWork.Index << FWork.Range);
	_waitForState = state;
	_state = drsWait;
	// Reset the timer to generate an error when waiting takes too long.
	_timeoutTimer.reset();
	// Always return true for being able to use it as a return value.
	return true;
}

bool ResultDataRequester::requestIndex(const Range& range)
{
	// Cannot accept new request when not ready.
	if (_state != drsReady || range.isEmpty())
	{
		return false;
	}// SetError("Not ready to accept an other request");
	// Check if the requested range is in the accessible range.
	if (!_rdIndex->getAccessRange().isWithinOther(range))
	{
		return setError("Index request was invalid");
	}
	// Set the new range to work on.
	_work._index = range;
	// Trigger the state machine to go to work.
	if (setState(drsGetIndex))
	{ // This could speed up things.
		process();
		return true;
	}
	return false;
}

bool ResultDataRequester::requestData(const Range& range)
{
	// Cannot accept new request when not ready and the range is empty.
	if (_state != drsReady || range.isEmpty())
	{
		return false;
	}
	// Set the index value to an impossible value.
	_work._index.clear();
	// Set the work range.
	_work._range = range;
	// Trigger the state machine to go to work on the data and skip
	// getting the index result.
	if (setState(drsTryData))
	{ // This could speed up things.
		process();
		return true;
	}
	return false;
}

bool ResultDataRequester::process()
{
	//
	switch (_state)
	{
		case drsError:
			return false;

		case drsReady:
			return true;

		case drsGetIndex:
			// Check if the range is valid if not request it.
			if (!_rdIndex->isIndexRangeValid(_work._index))
			{
				// Set the work this index result for when a request
				// is handled immediately and will reenter during the request call.
				_work._indexRequest = true;
				// Request the needed range and check for an error.
				if (!_rdIndex->requestIndexRange(_work._index))
				{
					_work._indexRequest = false;
					return setError("Index request failed");
				}
			}
			// When requests were made and thus the data was not valid yet we must wait.
			if (_work._indexRequest)
			{
				// Make the state machine wait for the request events.
				return waitForState(drsReadIndex);
			}

		case drsReadIndex:
			// Data was valid so read the index data into the data range.
			if (!_rdIndex->readIndexRange(_work._index, _work._range))
			{
				return setError("Index read failed");
			}
			//_RTTI_NOTIFY(DO_DBGSTR, "INDEXREAD: Work" << FWork.Index << FWork.Range)
			// When the data was available waiting is not needed.

		case drsTryData:
			// Check data pointed to by the retrieved range is accessible at the moment.
			for (unsigned i = 0; i < _rdDataList.count(); i++)
			{
				// Results which are not linked are ignored.
				if (_rdDataList[i]->getId())
				{
					Range rng = _rdDataList[i]->getAccessRange();
					if (!rng.isWithinOther(_work._range))
					{
						_work._dataAccess.Set(i);
					}
				}
				else
				{
					_work._dataAccess.Unset(i);
				}
			}
			// If all needed data was not accessible it must be waited for.
			if (_work._dataAccess.Bits)
			{
				// If not wait for it.
				return waitForState(drsGetData);
			}

		case drsGetData:
			for (unsigned i = 0; i < _rdDataList.count(); i++)
			{
				// Results which are not linked are ignored.
				if (_rdDataList[i]->getId())
				{
					// Check if the range is valid if not request it.
					if (!_rdDataList[i]->isRangeValid(_work._range))
					{
						// Set the work on bit for this data result for when a request
						// is handled immediately and will reenter during the request call.
						_work._dataRequest.Set(i);
						// Request the needed range and check for an error.
						if (!_rdDataList[i]->requestRange(_work._range))
						{
							_work._dataRequest.Unset(i);
							return setError("Data '" + _rdDataList[i]->getName() + "' request failed");
						}
					}
				}
				else
				{
					_work._dataRequest.Unset(i);
				}
			}
			// When requests were made and thus the data was not valid yet we
			// must wait for it to get valid.
			if (_work._dataRequest.Bits)
			{
				// Make the state machine wait for the request events.
				return waitForState(drsApply);
			}

		case drsApply:
			// Allow a new request to be placed again.
			if (!setState(drsReady))
			{
				return false;
			}
			else
			{
				bool ok = true;
				// Check if the index range is valid.
				if (!_work._index.isEmpty())
				{
					ok &= _rdIndex->isRangeValid(_work._index);
				}
				// Iterate through all attached synchronous data instances to check if the range is valid.
				for (unsigned i = 0; i < _rdDataList.count(); i++)
				{
					// Results which are not linked are ignored.
					if (_rdDataList[i]->getId())
					{
						ok &= _rdDataList[i]->isRangeValid(_work._range);
					}
				}
				if (!ok)
				{
					setError("Design flaw");
				}
				// Signal that the requested data is valid.
				passIndexEvent(reDataValid);
			}
			break;

		case drsWait:
		{
			// When the wait timer timed out generate an error.
			if (_timeoutTimer)
			{
				// Disable timer to prevent reentry.
				_timeoutTimer.disable();
				// Check which result caused the timeout.
				for (unsigned i = 0; i < _rdDataList.count(); i++)
				{
					if (_work._dataRequest.Has(i)) SF_RTTI_NOTIFY(DO_DEFAULT, "Data Req " << _work._range
						<< " of result '" << _rdDataList[i]->getName(2) << "' " << _rdDataList[i]->getBlockCount())
					if (_work._dataAccess.Has(i)) SF_RTTI_NOTIFY(DO_DEFAULT, "Data Access " << _work._range
						<< " of result '" << _rdDataList[i]->getName(2) << "' " << _rdDataList[i]->getBlockCount())
				}
				// Set error state.
				return setError("Wait timer timed out");
			}
			//
			switch (_waitForState)
			{
				case drsApply:
					// If requests are still out continue waiting.
					if (_work._dataRequest.Bits)
					{
						break;
					}
					// Enter the next state.
					if (setState(_waitForState))
					{
						return process();
					}
					else
					{
						return false;
					}

				case drsReadIndex:
					// If request is still out continue waiting.
					if (_work._indexRequest)
					{
						break;
					}
					// Enter the next state.
					if (setState(_waitForState))
					{
						return process();
					}
					else
					{
						return false;
					}

				case drsGetData:
					// Continue as long as the bits of the request are not
					// zeroed by the result event handler.
					if (_work._dataAccess.Bits)
					{
						break;
					}
					// Enter the next state.
					if (setState(_waitForState))
					{
						return process();
					}
					else
					{
						return false;
					}

				default:
					// Cannot wait for any other state.
					return setError("Impossible state to wait for");
			}
		}
			break;
	}
	return true;
}

} // namespace