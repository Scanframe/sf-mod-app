#pragma once
#include <functional>

namespace sf
{

/**
 * @brief Encapsulates the std::function() template
 * @tparam Result Return type of the function.
 * @tparam Args Variable amount of function arguments.
 */
template<typename Result, typename... Args>
class TClosure
{
	public:
		/**
		 * @brief Function type of the lambda or function.
		 */
		typedef std::function<Result(Args...)> func_type;

		/**
		 * @brief Default constructor.
		 */
		TClosure() = default;

		/**
		 * @brief Copy constructor.
		 */
		TClosure(const TClosure& c);

		/**
		 * @brief Function assignment constructor.
		 */
		explicit TClosure(const func_type& fn);

		/**
		 * @brief Function assignment method for static function.
		 */
		TClosure& assign(const func_type& fn);

		/**
		 * @brief Binds a non-static class member function to this instance.
		 *
		 * Short for passing a std::bind(...) result the #assign().<br>
		 * For example:
		 * @code
		 * bool MyClass::method(const char* s, int i);
		 *
		 * TClosure<bool, const char*, int> c2
		 *
		 * c2.assign(&MyClass::method, this, std::placeholders::_1, std::placeholders::_2)
		 * @endcode
		 *
		 * @tparam ClassType Type having the to be called method.
		 * @tparam MethodType Type from the passed class type.
		 * @tparam BoundArgs Only std::placeholders::_? enumeration values. (see std::bind)
		 *
		 * @param cls Pointer of the class having the passed method.
		 * @param mtd Pointer to method of the given class.
		 * @param args Amount of arguments needed to call the method using **std::placeholders::_?** enumeration values.
		 * @return Itself.
		 */
		template<typename ClassType, typename MethodType, typename... BoundArgs>
		TClosure& assign(ClassType* cls, MethodType mtd, BoundArgs... args);

		/**
		 * @brief Releases the bind function.
		 */
		TClosure& unassign();

		/**
		 * @brief Closure assignment operator.
		 */
		TClosure& operator=(const TClosure& c);

		/**
		 * @brief Closure assignment operator.
		 */
		TClosure& operator=(const func_type& f);

		/**
		 * @brief Make the call to the member function.
		 * @param args List of arguments specified by the template.
		 * @return Result type
		 */
		Result operator()(Args... args) const;

		/**
		 * @brief Checks if the closure is valid for calling.
		 */
		[[nodiscard]] bool isAssigned() const;

		/**
		 * @brief Checks if the closure is valid for calling.
		 */
		explicit operator bool() const;

	protected:
		/**
		 * @brief Makes the call to the member function.
		 * @param args List of arguments specified by the template.
		 * @return Result type
		 */
		Result call(Args... args) const;

	private:
		/**
		 * @brief Member holding the function.
		 */
		func_type _func{nullptr};
};

}// namespace sf

#include <misc/gen/TClosure.hpp>
