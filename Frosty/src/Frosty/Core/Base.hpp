#ifndef BASE_HPP
#define BASE_HPP

#ifdef FY_DEBUG
	#define FY_ENABLE_ASSERTS
	#define FY_ENABLE_NEW_OVERLOAD
	#define FY_ENABLE_TIME
#endif

#ifdef FY_RELEASE
	#define FY_ENABLE_TIME
#endif

#ifdef FY_ENABLE_NEW_OVERLOAD
	#define FY_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
	#define FY_NEW new
#endif

#ifdef FY_ENABLE_TIME
	#define FY_TIME(x, name) { Frosty::Time::StartTimer(name); x; Frosty::Time:EndTimer(name); }
#else
	#define FY_TIME(x, name) { x; }
#endif 

#ifdef FY_ENABLE_ASSERTS
	#define FY_ASSERT(x, ...) { if(!(x)) { FY_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define FY_CORE_ASSERT(x, ...) { if(!(x)) { FY_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define FY_ASSERT(x, ...)
	#define FY_CORE_ASSERT(x, ...)
#endif

#define FY_TIME_DISABLE(x, name) { x; }
#define BIT(x) (1 << x)

#endif 
