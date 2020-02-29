#ifndef FILE_CRESS_DEFER_HPP
#define FILE_CRESS_DEFER_HPP
/*
//	Author:		Elijah Shadbolt
//	Date:		29 Feb 2020
//	Licence:	MIT
//	GitHub:		https://github.com/Cresspresso/defer
//
//	Summary:
//		Defers execution of statements to the end of the scope.
//		Compatible with C++11, C++14, C++17.
//
//	Tips:
//		Use `std::unique_ptr` with custom deleter to return
//		RAII-wrapped non-pointer types from functions.
*/

#include <memory>

namespace cress
{
	/*	Example:
		```
			int* raw_ptr = new int(5);
			auto const my_callable = [&]()noexcept{
				delete raw_ptr;
				raw_ptr = nullptr;
			};
			cress::Defer<decltype(my_callable)> const my_defer(my_callable);
			// ... do stuff with raw_ptr ...
		```
		Example 2 (C++17):
		```
			int* raw_ptr = new int(5);
			auto const my_defer = cress::Defer([&]()noexcept{
				delete raw_ptr;
				raw_ptr = nullptr;
			});
			// ... do stuff with raw_ptr ...
		```
	*/
	template<class Callable>
	struct Defer
	{
	private:
		Callable m_callable;
	public:
		Defer(Defer const&) = delete;
		Defer(Defer&&) noexcept = delete;
		Defer& operator=(Defer const&) = delete;
		Defer& operator=(Defer&&) noexcept = delete;
		~Defer() noexcept { m_callable(); }
		Defer() : m_callable() {}
		explicit Defer(Callable callable) : m_callable(std::move(callable)) {}
	};
}


#define DETAIL_CRESS_DEFER_CAT_IMPL(a, b) a ## b
#define DETAIL_CRESS_DEFER_CAT(a, b) DETAIL_CRESS_DEFER_CAT_IMPL(a, b)
#define DETAIL_CRESS_DEFER_LINEGENNAME DETAIL_CRESS_DEFER_CAT(line, __LINE__)


/*	Example:
	```
		int* raw_ptr = new int(5);
		CRESS_DEFER_BEGIN(random_name) {
			delete raw_ptr;
			raw_ptr = nullptr;
		} CRESS_DEFER_END(random_name);
		// ... do stuff with raw_ptr ...
	```
*/
#define CRESS_DEFER_BEGIN(name)\
	auto DETAIL_CRESS_DEFER_CAT(CRESS_deferfn_, name) = [&]()noexcept
#define CRESS_DEFER_END(name)\
	;\
	::cress::Defer<decltype(DETAIL_CRESS_DEFER_CAT(CRESS_deferfn_, name))>\
		DETAIL_CRESS_DEFER_CAT(CRESS_defer_, name)(\
			::std::move(DETAIL_CRESS_DEFER_CAT(CRESS_deferfn_, name)));\
	do{}while(0)


/*	Example:
	```
		int* raw_ptr = new int(5);
		CRESS_DEFER_BLOCK({ delete raw_ptr; });
		// ... do stuff with raw_ptr ...
	```
*/
#define CRESS_DEFER_BLOCK(block, ...)\
	CRESS_DEFER_BEGIN(DETAIL_CRESS_DEFER_LINEGENNAME)\
	block __VA_ARGS__\
	CRESS_DEFER_END(DETAIL_CRESS_DEFER_LINEGENNAME)


/*	Example:
	```
		int* raw_ptr = new int(5);
		auto my_callable = [&]()noexcept{
			delete raw_ptr;
			raw_ptr = nullptr;
		};
		CRESS_DEFER_CALLABLE_VARIABLE(my_callable);
		// ... do stuff with raw_ptr ...
	```
*/
#define CRESS_DEFER_CALLABLE_VARIABLE(callable_variable)\
	cress::Defer<decltype(callable_variable)> const\
		DETAIL_CRESS_DEFER_CAT(CRESS_defer_, DETAIL_CRESS_DEFER_LINEGENNAME)\
			(::std::move(callable_variable));



#if ((__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) && (_MSC_VER >= 1913)))
#define DETAIL_CRESS_DEFER_HAS_CPP17 1
#else
#define DETAIL_CRESS_DEFER_HAS_CPP17 0
#endif

#if DETAIL_CRESS_DEFER_HAS_CPP17


/*	Example:
	```
		int* raw_ptr = new int(5);
		CRESS_DEFER_BEGIN_CPP17 {
			delete raw_ptr;
			raw_ptr = nullptr;
		} CRESS_DEFER_END_CPP17;
		// ... do stuff with raw_ptr ...
	```
*/
#define CRESS_DEFER_BEGIN_CPP17\
	auto const DETAIL_CRESS_DEFER_CAT(CRESS_defer_, DETAIL_CRESS_DEFER_LINEGENNAME)\
		= ::cress::Defer([&]()noexcept

#define CRESS_DEFER_END_CPP17\
	); do{}while(0)


#endif // ^^^ CRESS_DEFER_CPP17_ENABLED

#endif // ^^^ FILE_CRESS_DEFER_HPP
