#include "ResultDataRequester.h"
#include "ResultData.h"
#include "ResultDataHandler.h"
#include <misc/gen/dbgutils.h>

namespace sf
{

ResultDataRequester::ResultDataRequester()
	: _sustain(this, &ResultDataRequester::sustain, _sustain.spTimer)
	, _handlerData(this, &ResultDataRequester::resultCallback)
	// Default time for state machine to wait for a state change.
	, _timeoutTimer({1, 0})
	, _handler(nullptr)
	, _rdIndex(nullptr)
	// Is used for debugging timing of the state machine.
	, _byPass(false)
{
	// Timer is just needed for testing the timeout.
	_sustain.setInterval({1, 0});
}

ResultDataRequester::~ResultDataRequester()
{
	release();
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

void ResultDataRequester::setHandler(ResultDataHandler* handler)
{
	_handler = handler;
}

void ResultDataRequester::release()
{
	// Release the index result-data.
	if (_rdIndex)
	{
		_rdIndex->setHandler(nullptr);
		_rdIndex = nullptr;
	}
	// Release the index result-data.
	for (auto rd: _rdDataList)
	{
		rd->setHandler(nullptr);
	}
	// Clear the list of data results.
	_rdDataList.clear();
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
			SF_COND_RTTI_NOTIFY(rd->getHandler(), DO_CLOG, "Index result is already linked to a handler!")
			// Link the handler.
			rd->setHandler(&_handlerData);
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
		if (_rdDataList.find(rd) != InformationTypes::npos)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Tried to assign data result twice!")
			return;
		}
		// Add to the list of data results.
		_rdDataList.add(rd);
		//
		rd->setHandler(&_handlerData);
		// Reset the state machine when a change was eminent.
		reset();
		// Assign the index of the result list in the result's data field.
		for (unsigned i = 0; i < _rdDataList.count(); i++)
		{
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
		SF_COND_RTTI_NOTIFY(rd->getHandler() != &_handlerData, DO_CLOG, "Linked handler was reassigned!")
		// Unlink the handler.
		rd->setHandler(nullptr);
		// Remove result pointer from the list.
		_rdDataList.detachAt(idx);
		// Reset the state machine when a change was eminent.
		reset();
	}
}

void ResultDataRequester::resultCallback(
	ResultDataTypes::EEvent event,
	const ResultData& caller,
	ResultData& link,
	const Range& rng,
	bool sameInst
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
			default:
				break;

			case ResultData::reIdChanged:
			case ResultData::reClear: {
				// When an id changes the state machine must be reset.
				reset();
				break;
			}

			case ResultData::reAccessChange:
				if (&link != _rdIndex)
				{
					if (!_work._dataAccess._bits || !link.getId())
					{
						break;
					}
					// Check if this result has to catch up.
					if (_work._dataAccess.has(link.getData<int>()))
					{
						// Check if the result has caught up with the working range of the index.
						if (rng.isWithinSelf(_work._range))
						{
							_work._dataAccess.unset(link.getData<int>());
						}
						// Speed up thing by calling process when waiting is over.
						if (!_work._dataAccess._bits)
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
						setError("Index GotRange event not expected");
					}
					// Reset the flag.
					_work._indexRequest = false;
					// Speed up thing by calling process.
					process();
				}
				else
				{
					if (!_work._dataRequest.has(link.getData<int>()))
					{
						setError("Data GotRange event not expected");
					}
					// Clear the bit of the request placed.
					_work._dataRequest.unset(link.getData<int>());
					// Speed up thing by calling process wen waiting is over.
					if (!_work._dataRequest._bits)
					{
						process();
					}
					break;
				}
		}
	}
	// Emit the event.
	if (_handler)
	{
		_handler->resultDataEventHandler(event, caller, link, rng, sameInst);
	}
}

void ResultDataRequester::passIndexEvent(ResultDataRequester::EReqEvent event)
{
	if (_handler)
	{
		_handler->resultDataEventHandler(
			(ResultData::EEvent) event,
			*_rdIndex,
			*_rdIndex,
			_rdIndex->getId() ? _work._index : _work._range,
			true
		);
	}
}

bool ResultDataRequester::sustain(const timespec&)
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
	if (state == -2)
	{
		state = _state;
	}
	switch (state)
	{
		case drsError:
			return "Error";
		case drsReady:
			return "Ready";
		case drsGetIndex:
			return "GetIndex";
		case drsReadIndex:
			return "ReadIndex";
		case drsGetData:
			return "GetData";
		case drsApply:
			return "Apply";
		case drsTryData:
			return "TryData";
		case drsWait:
			return "Wait";
		default:
			return "Unknown";
	}
}

bool ResultDataRequester::setState(ResultDataRequester::EState state)
{
	if (_state == drsError)
	{
		return false;
	}
	// Only change the previous state when the state is not psWAIT.
	if (_state != drsWait)
	{
		_statePrevious = _state;
	}
	// Assign the new state value.
	_state = state;
	// No waiting when state was set with this function.
	_stateWait = drsReady;
	// Signal success.
	return true;
}

bool ResultDataRequester::setError(const std::string& text)
{
	auto old_prev = _statePrevious;
	// Update the previous state.
	if (_state != drsWait)
	{
		_statePrevious = _state;
	}
	// Assign the new state first so that msg boxes can appear.
	_state = drsError;
	// Do some debug printing in case of an error.
	SF_RTTI_NOTIFY(DO_DEFAULT, "State Machine ran into an error '" << text << "'. SetState(" << getStateName(old_prev) << "=>" << getStateName(_statePrevious) << "=>" << getStateName(_state) << ")" << _work._index << _work._range)
	//
	SF_RTTI_NOTIFY(DO_DEFAULT, "Resetting the state machine.")
	// Reset the state machine after each error.
	reset();
	// Always return false for being able to use it as a return value.
	return false;
}

bool ResultDataRequester::waitForState(ResultDataRequester::EState state)
{
	SF_RTTI_NOTIFY(DO_CLOG, "WaitForState(" << getStateName(state) << ")" << _work._index << _work._range);
	_stateWait = state;
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
	}
	// SetError("Not ready to accept another request");
	// Check if the requested range is in the accessible range.
	if (!_rdIndex->getAccessRange().isWithinSelf(range))
	{
		return setError("Index request was invalid");
	}
	// Set the new range to work on.
	_work._index = range;
	// Trigger the state machine to go to work.
	if (setState(drsGetIndex))
	{
		// This could speed up things.
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
	{// This could speed up things.
		process();
		return true;
	}
	return false;
}

bool ResultDataRequester::process()// NOLINT(misc-no-recursion)
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
			//SF_RTTI_NOTIFY(DO_CLOG, "Index read: Work" << _work._index << _work._range)
			// When the data was available waiting is not needed.

		case drsTryData:
			// Check data pointed to by the retrieved range is accessible at the moment.
			for (int i = 0; i < _rdDataList.count(); i++)
			{
				// Results which are not linked are ignored.
				if (_rdDataList[i]->getId())
				{
					if (!_rdDataList[i]->getAccessRange().isWithinSelf(_work._range))
					{
						_work._dataAccess.set(i);
					}
				}
				else
				{
					_work._dataAccess.unset(i);
				}
			}
			// If all needed data was not accessible it must be waited for.
			if (_work._dataAccess._bits)
			{
				// If not wait for it.
				return waitForState(drsGetData);
			}

		case drsGetData:
			for (int i = 0; i < _rdDataList.count(); i++)
			{
				// Results which are not linked are ignored.
				if (_rdDataList[i]->getId())
				{
					// Check if the range is valid if not request it.
					if (!_rdDataList[i]->isRangeValid(_work._range))
					{
						// Set the work on bit for this data result for when a request
						// is handled immediately and will reenter during the request call.
						_work._dataRequest.set(i);
						// Request the needed range and check for an error.
						if (!_rdDataList[i]->requestRange(_work._range))
						{
							_work._dataRequest.unset(i);
							return setError("Data '" + _rdDataList[i]->getName() + "' request failed");
						}
					}
				}
				else
				{
					_work._dataRequest.unset(i);
				}
			}
			// When requests were made and thus the data was not valid yet we
			// must wait for it to get valid.
			if (_work._dataRequest._bits)
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

		case drsWait: {
			// When the wait timer timed out generate an error.
			if (_timeoutTimer)
			{
				// Disable timer to prevent reentry.
				_timeoutTimer.disable();
				// Check which result caused the timeout.
				for (int i = 0; i < _rdDataList.count(); i++)
				{
					if (_work._dataRequest.has(i))
					{
						SF_RTTI_NOTIFY(DO_DEFAULT, "Data Req " << _work._range << " of result '" << _rdDataList[i]->getName(2) << "' " << _rdDataList[i]->getBlockCount())
					}
					if (_work._dataAccess.has(i))
					{
						SF_RTTI_NOTIFY(DO_DEFAULT, "Data Access " << _work._range << " of result '" << _rdDataList[i]->getName(2) << "' " << _rdDataList[i]->getBlockCount())
					}
				}
				// Set error state.
				return setError("Wait timer timed out");
			}
			//
			switch (_stateWait)
			{
				case drsApply:
					// If requests are still out continue waiting.
					if (_work._dataRequest._bits)
					{
						break;
					}
					// Enter the next state.
					if (setState(_stateWait))
					{
						return process();
					}
					else
					{
						return false;
					}

				case drsReadIndex:
					// If request is still out, continue waiting.
					if (_work._indexRequest)
					{
						break;
					}
					// Enter the next state.
					if (setState(_stateWait))
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
					if (_work._dataAccess._bits)
					{
						break;
					}
					// Enter the next state.
					if (setState(_stateWait))
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
		} break;
	}
	return true;
}

std::ostream& ResultDataRequester::getStatus(std::ostream& os)
{
	os << "State (prev > cur): " << getStateName(_statePrevious) << " > " << getStateName(_state) << std::endl
		 << "Work Index: " << _work._index << std::endl
		 << "Work Range: " << _work._range << std::endl;
	return os;
}

}// namespace sf