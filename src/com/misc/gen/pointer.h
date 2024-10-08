#pragma once

#include "../global.h"
#include <utility>

namespace sf
{

/**
 * @brief Deletes object and clears pointer
 *
 * Defines and templates for deleting allocated memory and
 * testing for zero and clearing the pointer at the same time.
 */
template<class T>
inline void delete_null(T& p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}

/**
 * @brief Template function deleting an array previous allocated by 'new[]' when the pointer is non-null and also nulls the passed pointer.
 */
template<class T>
inline void delete_anull(T& p)
{
	if (p)
	{
		delete[] p;
		p = nullptr;
	}
}

/**
 * @brief Template function freeing a pointer previous allocated by 'malloc' when the pointer is non-null and also nulls the passed pointer.
 */
template<class T>
inline void free_null(T& p)
{
	if (p)
	{
		free(p);
		p = nullptr;
	}
}

/**
 * @deprecated Use std::swap()
 * @brief Swaps the passed two arguments of any type.
 *
 * @tparam T Type that is swapped
 * @param t1 Parameter 1 of type
 * @param t2 Parameter 2 of type
 */
template<class T>
[[deprecated]] inline void swap_it(T& t1, T& t2)
{
	std::swap(t1, t2);
}

/**
 * @brief Deletes the pointer of type T allocated by 'new' when this instance goes out of scope.
 * @tparam T
 */
template<class T>
class scope_delete
{
	public:
		explicit scope_delete(T* p) { P = p; }

		~scope_delete() { delete_null(P); }

		inline void disable_delete() { P = nullptr; }

	private:
		T* P;
};

/**
 * @brief Frees the pointer of type T allocated by 'malloc' when this instance goes out of scope.
 * @tparam T
 */
template<class T>
class scope_free
{
	public:
		explicit scope_free(T* p) { P = p; }

		~scope_free() { free_null(P); }

		inline void disable_free() { P = nullptr; }

	private:
		T* P;
};

/**
 * Creates typed reference to a null pointer.
 * @tparam T
 * @return
 */
template<typename T>
inline T& null_ref()
{
	return (*(static_cast<T*>(nullptr)));
}

template<typename T>
inline bool not_ref_null(T& r)
{
	return &r == nullptr;
}

}// namespace sf
