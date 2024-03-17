#pragma once

#include <misc/gen/ElapseTimer.h>
#include <misc/gen/Sustain.h>
#include <misc/gen/TBitSet.h>
#include "ResultData.h"

namespace sf
{

/**
 * @brief Handles requests and events for indexed data sources.
 *
 * #sf::ResultData instances can be attached to this type of instance to handle
 * request on multiple results for when all data must be available at a time for processing.
 */
class _GII_CLASS ResultDataRequester :public ResultDataTypes
{
	public:
		/**
		 * @brief Additional local user result data events.
		 */
		enum EReqEvent
		{
			/**
			 * Requested data is valid and the passed.
			 * Range contains the requested index range.
			 */
			reDataValid = reUserLocal,
			/**
			 * State machine timed out on getting data.
			 * The passed range contains the requested index range.
			 */
			reTimedOut = reUserLocal + 1,
		};

		/**
		 * @brief Default constructor.
		 */
		ResultDataRequester();

		/**
		 * @brief Virtual destructor.
		 */
		virtual ~ResultDataRequester();

		/**
		 * @brief Links the single index result-data entry to this instance.
		 */
		void attachIndex(ResultData* rd);

		/**
		 * @brief Hooks the result-data entries to this instance.
		 */
		void attachData(ResultData* rd);

		/**
		 * @brief Unhooks the result-data entries to this instance.
		 */
		void detachData(ResultData* rd);

		/**
		 * @brief Releases all the result-data entries and set handlers.
		 *
		 * When both result-data instances and the requester are part of a class.
		 * This method must called from the owners destructor to prevent crashing.
		 */
		void release();

		/**
		 * @brief Requests a range of the data using a single index range.
		 * @return True on success.
		 */
		inline bool requestIndex(size_type index);

		/**
		 * @brief Requests a range of the data using multiple index ranges.
		 * @param range
		 * @return True on success.
		 */
		bool requestIndex(const Range& range);

		/**
		 * @brief Directly requests a range of the data without using the index.
		 * @param range
		 * @return True on success.
		 */
		bool requestData(const Range& range);

		/**
		 * @brief Sets the time-out in ms in which a request is allowed too take before it is timed out.
		 * A value of zero. makes it wait indefinitely.
		 */
		inline void setTimeout(const TimeSpec& timeout);

		/**
		 * @brief Called when results are attached or detached and when a time-out occurred.
		 * Sets all members to their initial state.
		 */
		void reset();

		/**
		 * @brief Gets the index range the requester is working on or has done.
		 */
		[[nodiscard]] inline const Range& getIndexRange() const;

		/**
		 * @brief Gets the data range the requester is working on or has done.
		 */
		[[nodiscard]] inline const Range& getDataRange() const;

		/**
		 * @brief Sets an event handler for this instance to which all result events are rerouted after it handled its own stuff.
		 *
		 * @param handler Passing nullptr wil disable the link.
		 */
		void setHandler(ResultDataHandler* handler);

		/**
		 * @brief Returns the linked handler hook object.
		 */
		[[nodiscard]] inline const ResultDataHandler* getHandler() const;

		/**
		 * @brief Enumerate for states.
		 */
		enum EState
		{
			/** Error state for debugging.*/
			drsError = -1,
			/** Ready for request.*/
			drsReady,
			/** Trying to get a valid new index.*/
			drsGetIndex,
			/** Reads the index result for the data range.*/
			drsReadIndex,
			/** Checks to data accessible ranges.*/
			drsTryData,
			/** Trying to get a valid new data.*/
			drsGetData,
			/** Apply the retrieved data.*/
			drsApply,
			/** Waiting a data event or timeout to occur.*/
			drsWait,
		};

		/**
		 * @brief Gets the name of the state.
		 *
		 * @param state When -2 is passed the current state is returned.
		 * @return String of the state.
		 */
		const char* getStateName(int state = -2);

		/**
		 * Gets the current state from the requester.
		 */
		[[nodiscard]] inline EState getState() const;

		/**
		 * @brief For debugging purposes only it writes the status to the output stream.
		 */
		std::ostream& getStatus(std::ostream& os);

	private:
		/**
		 * @brief Hook to sustain interface.
		 */
		TSustain<ResultDataRequester> _sustain;

		/**
		 * @brief Called from sustain interface.
		 */
		bool sustain(const timespec& t);

		/**
		 * @brief Hook to result events.
		 */
		TResultDataHandler<ResultDataRequester> _handlerData;

		/**
		 * @brief Result data event handler.
		 */
		void resultCallback(ResultDataTypes::EEvent event, const ResultData& caller, ResultData& link, const Range& rng, bool sameInst);

		/**
		 * @brief Index result.
		 */
		ResultData* _rdIndex{nullptr};
		/**
		 * @brief Data results linked.
		 */
		PtrVector _rdDataList;
		/**
		 * @brief Timer implementation for timing out a request.
		 */
		ElapseTimer _timeoutTimer;

		/**
		 * @brief Processes
		 */
		bool process();

		/**
		 * @brief Holds the current state.
		 */
		EState _state{drsReady};
		/**
		 * @brief Holds the previous state.
		 */
		EState _statePrevious{drsReady};
		/**
		 * @brief State which the sustain will move to requests are met.
		 */
		EState _stateWait{drsReady};

		/**
		 * @brief Sets the new state and acts on it.
		 */
		bool setState(EState state);

		/**
		 * @brief Sets the error state.
		 * @param text Error text.
		 * @return Always false for ease in functions.
		 */
		bool setError(const std::string& text);

		/**
		 * @brief Sets the wait state member to the passed value and the state machine
		 * waits for some conditions before continuing by setting the passed state.
		 * When conditions are not met in time an error is generated.
		 * When reset is true the timer is reset.
		 */
		bool waitForState(EState state);

		/**
		 * @brief Calls event handler passing own local events.
		 * @param event
		 */
		void passIndexEvent(EReqEvent event);

		/**
		 * @brief Structure holding data to work on.
		 */
		struct WorkData
		{
			/**
			 * @brief Default constructor.
			 */
			WorkData()
			{
				clear();
			}

			/**
			 * @brief Clears all dat members to their initial state.
			 */
			void clear()
			{
				_indexRequest = false;
				_dataRequest.Bits = 0;
				_range.clear();
				_index.clear();
				_dataRequest.Bits = 0;
				_dataAccess.Bits = 0;
			}

			/**
			 * @brief Data range of a request.
			 */
			Range _range;
			/**
			 * @brief Index range of a request.
			 */
			Range _index;
			/**
			 * @brief True when a request index was made.
			 */
			bool _indexRequest{};
			/**
			 * @brief True when a request data was made and it must be waited for.
			 */
			TSet<int> _dataRequest;
			/**
			 * @brief True when it must wait for results to catch up with the index result.
			 */
			TSet<int> _dataAccess;
		};

		/**
		 * @brief Holds request working/status information.
		 */
		WorkData _work;
		/**
		 * @brief Pointer to ResultDataEventHandler function.
		 */
		ResultDataHandler* _handler;
		/**
		 * @brief Used to enable by pass of event handling. Can be enabled when debugging.
		 */
		bool _byPass;
};

inline
ResultDataRequester::EState ResultDataRequester::getState() const
{
	return _state;
}

inline
void ResultDataRequester::setTimeout(const TimeSpec& timeout)
{
	_timeoutTimer.set(timeout);
}

inline
bool ResultDataRequester::requestIndex(size_type index)
{
	return requestIndex(Range(index, index + 1));
}

inline
const ResultDataHandler* ResultDataRequester::getHandler() const
{
	return _handler;
}

inline
const Range& ResultDataRequester::getIndexRange() const
{
	return _work._index;
}

inline
const Range& ResultDataRequester::getDataRange() const
{
	return _work._range;
}

}
