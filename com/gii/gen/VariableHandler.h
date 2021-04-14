#pragma once

#include "VariableTypes.h"

namespace sf
{

/**
 * @brief Base class used for giving a Variable instance access to a member functions
 * of a derived class. This class is not used directly and is used a base class
 * for the TVariableHandler<T> template class.
 */
struct _GII_CLASS VariableHandler :public VariableTypes
{
	/**
	 * @brief Pure virtual function which must be implemented when used in a polymorphic setting.
	 *
	 * @param event Type of the event
	 * @param call_var Calling variable instance
	 * @param link_var Linked variable instance
	 * @param same_inst
	 */
	virtual void VariableEventHandler
		(
			EEvent event,
			const Variable& call_var,
			Variable& link_var,
			bool same_inst
		) = 0;

	/**
	 * @brief Destructor clears the link with variable instances so no errors occur
	 * when the link is destructed before the variable is.
	 */
	virtual ~VariableHandler();
};

/**
 * @brief Template for linking pointers of member function to Variable instances.
 *
 * @tparam T
 */
template<class T>
class TVariableHandler :public VariableHandler
{
	public:
		/**
		 * @brief Required event handler type.
		 */
		typedef void (T::*TPmf)(EEvent event, const Variable& call_var, Variable& link_var, bool same_inst);

		/**
		 * @brief Constructor for assigning the pointer of the member function.
		 */
		TVariableHandler(T* self, TPmf pmf);

		/**
		 * @brief Prevent copying by not implementing copy constructor.
		 */
		TVariableHandler(const TVariableHandler&) = delete;

		/**
		 * @brief Prevent copying by not implementing assignment operator.
		 */
		TVariableHandler& operator=(const VariableHandler&) = delete;

	private:
		/**
		 * @brief Pointer to member function.
		 */
		TPmf _pmf;
		/**
		 * @brief Pointer to class instance.
		 */
		T* _self;

		/**
		 * @brief Call the member function through virtual overloaded function from the base class.
		 *
		 * @param event
		 * @param call_var
		 * @param link_var
		 * @param same_inst
		 */
		void VariableEventHandler
			(
				EEvent event,
				const Variable& call_var,
				Variable& link_var,
				bool same_inst
			) override
		{
			(_self->*_pmf)(event, call_var, link_var, same_inst);
		}
};

template<class T>
inline
TVariableHandler<T>::TVariableHandler(T* self, TVariableHandler::TPmf pmf)
	:_self(self)
	 , _pmf(pmf)
{
}

}