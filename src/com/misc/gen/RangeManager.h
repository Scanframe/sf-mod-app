#pragma once

#include "../global.h"
#include "Range.h"

namespace sf
{

/**
 * @brief This class manages range requests made by clients identified by ID's in the ranges.<br>
 * @image html "doc/RangeManager-1.svg"
 *
 * A server sets the range which is managed.<br>
 * Multiple clients make requests for ranges passing a unique ID (Range::setId).<br>
 * The requested ranges are reduced to actual requests for the server by excluding ranges that are available already.<br>
 * A server retrieves the actual requests (#getActualRequests) and translates those to multiples of the used block size.<br>
 * When the blocks finished processing the server tells the range manager by calling #setAccessible .<br>
 * The manager updates the accessible ranges and rearrange them.
 * This means that overlapped and each other extending ranges are merged and sorted.<br>
 * At the same time the fulfilled client requests are extracted and removed from the manager.<br>
 * The server notifies clients identified by Range::getId.
 *
 * @see setManaged, getManaged, request, getRequests, getActualRequests, setAccessible, setAutoManaged
 * @see Range, Range::Vector
 */
class _MISC_CLASS RangeManager
{
	public:
		/**
		 * @brief Default constructor.
		 */
		RangeManager() = default;

		/**
		 * @brief Destructor.
		 */
		~RangeManager() = default;

		/**
		 * @brief Checks if flushing has any effect.
		 * @return True on having effect.
		 */
		bool isFlushable();

		/**
		 * @brief Flushes all ranges and range list as if it were initialized.
		 * @return True if a flush was actually performed.
		 */
		bool flush();

		/**
		 * @brief Sets the limiting range on requests.
		 * If the managed range is not set (empty) requests get the #rmOutOfRange result.
		 * Also for when the requested range is outside the managed range.
		 * @param r Managed range.
		 */
		void setManaged(const Range& r);

		/**
		 * @brief Retrieves the limit on the maximum requestable range span from zero.
		 * @return Managed range.
		 */
		[[nodiscard]] const Range& getManaged() const;

		/**
		 * @brief Retrieves the current accessible list of ranges.
		 * @return Access list.
		 */
		[[nodiscard]] const Range::Vector& getAccessibles() const;

		/**
		 * @brief Retrieves the current request list.
		 * @return Request list.
		 */
		[[nodiscard]] const Range::Vector& getRequests() const;

		/**
		 * @brief Retrieves actual request list of ranges missing to fulfill requests.<br>
		 * These are the resulting-requested ranges when all requests are added up.
		 * @returns Real request
		 */
		[[nodiscard]] const Range::Vector& getActualRequests() const;

		/**
		 * @brief Returns if this range is accessible. Using the managed range and the
		 * current accessible ranges as a reference.
		 * @param r Range
		 * @return True when accessible.
		 */
		[[nodiscard]] bool isAccessible(const Range& r) const;

		/**
		 * @brief Enumerate for return values used by #request method.
		 */
		enum EResult : int
		{
			/** Range was outside the managed range.*/
			rmOutOfRange = -1,
			/** Requested range is inaccessible and needs to be retrieved.*/
			rmInaccessible = 0,
			/** Returned when a requested range is already accessible.*/
			rmAccessible = 1
		};

		/**
		 * @brief Called by client to request access for a range.
		 * @param r Range to request access to. And the ID must be filled in.
		 * @param rrl Resulting ranges missing to fulfill the request.
		 * @return True when the request was excepted and added to the request range list.
		 *         False when the range is already accessible.
		 */
		EResult request(const Range& r, Range::Vector& rrl);

		/**
		 * @brief This function is called by a derived server class to as a reply to an event.
		 * The function will return all ranges and owners in an owned range list
		 * which request are satisfied/fulfilled with the passed ranges.
		 * @param rl New ranges that are accessible.
		 * @param rl_req Requested and fulfilled ranges becoming accessible now.
		 * @return True when ranges have become accessible.
		 */
		bool setAccessible(const Range::Vector& rl, Range::Vector& rl_req);

		/**
		 * @brief Same as #setAccessible but now for a single Range element.
		 * @param r Range that became accessible.
		 * @param rl_req Requested and fulfilled ranges becoming accessible now.
		 * @return True when ranges have become accessible.
		 */
		bool setAccessible(const Range& r, Range::Vector& rl_req);

		/**
		 * @brief Flushes all the requests having the range  identifier.
		 * @param id Identifier (Type can hold a pointer cast to the id_type).
		 */
		void flushRequests(Range::id_type id);

		/**
		 * @brief Determines if the managed range is automatically determined and set when #setAccessible is called.
		 * Making a call to #setManaged unnecessary when set to true.
		 * @param flag True when auto managed.
		 * @return Previous value of the flag.
		 */
		bool setAutoManaged(bool flag = true);

		/**
		 * @brief Test function for set privates during a unit test.
		 */
		void unitTest(Range::Vector* accessibles, Range::Vector* actual_requests = nullptr, Range::Vector* requests = nullptr);

		/**
		 * @brief Sets the debug flag for this instance.
		 *
		 * @param debug True to enable.
		 */
		void setDebug(bool debug);

	private:
		/**
		 * Determines if the Manged range is automatically incremented when setAccessibleRange is called.
		 */
		bool _autoManaged{false};
		/**
		 * contains the range that is managed.
		 */
		Range _managedRange;
		/**
		 * contains all ranges that are accessible.
		 */
		Range::Vector _accessibles;
		/**
		 * contains all initially made requests.
		 */
		Range::Vector _requests;
		/**
		 * contains the real requests made.
		 */
		Range::Vector _actualRequests;
		/**
		 * When true notify entries are produced in std::clog.
		 */
		bool _debug{false};
};

inline void RangeManager::setDebug(bool debug)
{
	_debug = debug;
}

inline bool RangeManager::setAutoManaged(bool flag)
{
	std::swap(flag, _autoManaged);
	return flag;
}

inline const Range& RangeManager::getManaged() const
{
	return _managedRange;
}

inline void RangeManager::setManaged(const Range& r)
{
	_managedRange = r;
}

inline const Range::Vector& RangeManager::getAccessibles() const
{
	return _accessibles;
}

inline const Range::Vector& RangeManager::getActualRequests() const
{
	return _actualRequests;
}

inline const Range::Vector& RangeManager::getRequests() const
{
	return _requests;
}

}// namespace sf