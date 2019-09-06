#ifndef LOG_HPP
#define LOG_HPP

#include "Base.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Frosty
{
	class Log
	{
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	};
}



#if defined(FY_RELEASE) || defined(FY_DEBUG)
	
	// Core log macros
	#define FY_CORE_TRACE(...)  ::Frosty::Log::GetCoreLogger()->trace(__VA_ARGS__)
	#define FY_CORE_INFO(...)   ::Frosty::Log::GetCoreLogger()->info(__VA_ARGS__)
	#define FY_CORE_WARN(...)   ::Frosty::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define FY_CORE_ERROR(...)  ::Frosty::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define FY_CORE_FATAL(...)  ::Frosty::Log::GetCoreLogger()->fatal(__VA_ARGS__)

	//Example FY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);

	// Client log macros
	#define FY_TRACE(...)       ::Frosty::Log::GetClientLogger()->trace(__VA_ARGS__)
	#define FY_INFO(...)        ::Frosty::Log::GetClientLogger()->info(__VA_ARGS__)
	#define FY_WARN(...)        ::Frosty::Log::GetClientLogger()->warn(__VA_ARGS__)
	#define FY_ERROR(...)       ::Frosty::Log::GetClientLogger()->error(__VA_ARGS__)
	#define FY_FATAL(...)       ::Frosty::Log::GetClientLogger()->fatal(__VA_ARGS__)

#else
	//////////////////////////////////////////////
	// REMOVE THIS ENTIRE SECTION ON FINAL DIST //
	//////////////////////////////////////////////

	// Core log macros
	#define FY_CORE_TRACE(...)
	#define FY_CORE_INFO(...) 
	#define FY_CORE_WARN(...) 
	#define FY_CORE_ERROR(...)
	#define FY_CORE_FATAL(...)

	//Example FY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);

	// Client log macros
#define FY_TRACE(...)
#define FY_INFO(...) 
#define FY_WARN(...) 
#define FY_ERROR(...)
#define FY_FATAL(...)

#endif
#endif // !LOG_HPP