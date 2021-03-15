#pragma once

#include <misc/gen/ElapseTimer.h>
#include <misc/gen/Sustain.h>
#include <misc/gen/TBitSet.h>
#include "ResultData.h"

namespace sf
{

/**
 * @brief Handles requests and events for indexed data sources.
 * #sf::ResultData instances can be attached to this type of instance to handle
 * request on multiple results for when all data must be available at a time for processing.
 */
class _GII_CLASS ResultDataRequester :public ResultDataTypes
{
	public:
		/**
		 * Additional local user result data events.
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
		 * Default constructor.
		 */
		ResultDataRequester();

		/**
		 * Virtual destructor.
		 */
		virtual ~ResultDataRequester();

		/**
		 * Links the single index result data entry to this instance.
		 * @param rd
		 */
		void attachIndex(ResultData* rd);

		/**
		 * Hooks the result data entries to this instance.
		 * @param rd
		 */
		void attachData(ResultData* rd);

		/**
		 * Unhooks the result data entries to this instance.
		 * @param rd
		 */
		void detachData(ResultData* rd);

		/**
		 * Requests a range of the data using a single index range.
		 * @return
		 */
		bool requestIndex(size_type index);

		/**
		 * Requests a range of the data using multiple index ranges.
		 * @param range
		 * @return
		 */
		bool requestIndex(const Range& range);

		/**
		 * Directly requests a range of the data without using the index.
		 * @param range
		 * @return
		 */
		bool requestData(const Range& range);

		/**
		 * Sets the time-out in ms in which a request is allowed too take
		 * before it is timed out. A value of zero. makes it wait indefinitely.
		 * @param timeout
		 */
		void setTimeout(clock_t timeout);

		/**
		 * Called when results are attached or detached and when a time-out occurred.
		 * Sets all members to their initial state.
		 */
		void reset();

		/**
		 * Returns the index range the requester is working on or has done.
		 * @return
		 */
		[[nodiscard]] const Range& getIndexRange() const;

		/**
		 * Returns the data range the requester is working on or has done.
		 * @return
		 */
		[[nodiscard]] const Range& getDataRange() const;

		/**
		 * Sets an event handler for this instance to which all result events
		 * are rerouted after it handled its own stuff.
		 * Passing NULL wil disable the link.
		 * @param link
		 */
		void SetLink(ResultDataHandler* link);

		/**
		 * Returns the linked handler hook object.
		 */
		[[nodiscard]] const ResultDataHandler* getLink() const;

	private:
		/**
		 * Hook to sustain interface.
		 */
		TSustain<ResultDataRequester> _sustain;

		/**
		 * Called from sustain interface.
		 * @param clk
		 * @return
		 */
		bool sustain(clock_t clk);

		/**
		 * Hook to result events.
		 */
		TResultDataLinkHandler<ResultDataRequester> _dataHandler;

		/**
		 * Result data event handler.
		 * @param event
		 * @param caller
		 * @param link
		 * @param rng
		 * @param same_inst
		 */
		void resultCallback
			(
				ResultDataTypes::EEvent event,
				const ResultData& caller,
				ResultData& link,
				const Range& rng,
				bool same_inst
			);

		/**
		 * Index result.
		 */
		ResultData* _rdIndex{nullptr};
		/**
		 * Data results.
		 */
		ResultData::Vector _rdDataList;
		/**
		 * Timer implementation for timing out a request.
		 */
		ElapseTimer _timeoutTimer;

		/**
		 * Processes
		 * @return
		 */
		bool process();

		/**
		 * Enumerate for state levels.
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
		 * Returns the name of the state.
		 * @param state
		 * @return
		 */
		const char* getStateName(int state);

		/**
		 * Holds the current state.
		 */
		EState _state{drsReady};
		/**
		 * Holds the previous state.
		 */
		EState _previousState{drsReady};
		/**
		 * State which the sustain will move to requests are met.
		 */
		EState _waitForState{drsReady};

		/**
		 * Sets the new state and acts on it.
		 * @param state
		 * @return
		 */
		bool setState(EState state);

		/**
		 * Sets the error state.
		 * Return always false for ease in functions.
		 * @param txt
		 * @return
		 */
		bool setError(const std::string& txt);

		/**
		 * Sets the wait state member to the passed value and the state machine
		 * waits for some conditions before continuing by setting the passed state.
		 * When conditions are not met in time an error is generated.
		 * When reset is true the timer is reset.
		 * @param state
		 * @return
		 */
		bool waitForState(EState state);

		/**
		 * Calls event handler passing own local events.
		 * @param event
		 */
		void passIndexEvent(EReqEvent event);

		/**
		 * Structure holding data to work on.
		 */
		struct WorkData
		{
			/**
			 * Default constructor.
			 */
			WorkData()
			{
				clear();
			}

			/**
			 * Clears all dat members to their initial state.
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
			 * Data range of a request.
			 */
			Range _range;
			/**
			 * Index range of a request.
			 */
			Range _index;
			/**
			 * True when a request index was made.
			 */
			bool _indexRequest{false};
			/**
			 * True when a request data was made and it must be waited for.
			 */
			TSet<int> _dataRequest;
			/**
			 * True when it must wait for results to catch up with the index result.
			 */
			TSet<int> _dataAccess;
		};

		/**
		 * Holds request working/status information.
		 */
		WorkData _work;
		/**
		 * Pointer to ResultDataEventHandler function.
		 */
		ResultDataHandler* _handler;
		/**
		 * Used to enable by pass of event handling. Can be enabled when debugging.
		 */
		bool _byPass;
};

inline
void ResultDataRequester::setTimeout(clock_t timeout)
{
	_timeoutTimer.set(timeout);
}

inline
bool ResultDataRequester::requestIndex(size_type  index)
{
	return requestIndex(Range(index, index + 1));
}

inline
const ResultDataHandler* ResultDataRequester::getLink() const
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

} // namespace
