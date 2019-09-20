workspace "SS-Project"
	architecture "x64"
	targetdir "build"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
	startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"
-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Frosty/vendor/GLFW/include"
IncludeDir["Glad"] = "Frosty/vendor/Glad/include"
IncludeDir["ImGui"] = "Frosty/vendor/imgui"
IncludeDir["glm"] = "Frosty/vendor/glm"
IncludeDir["Luna"] = "Frosty/vendor/Luna"

group "Dependencies"
	include "Frosty/vendor/GLFW"
	include "Frosty/vendor/Glad"
	include "Frosty/vendor/imgui"

group ""

project "Frosty"
	location "Frosty"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}") 
	
	pchheader "fypch.hpp"
	pchsource "Frosty/src/fypch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Luna}",
		"%{IncludeDir.glm}"
	}

	libdirs
	{
		"%{prj.name}/vendor/GLFW/lib",
		"%{prj.name}/vendor/Luna/lib"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"Luna.lib"
	}

	filter "system:windows"
		systemversion "latest"
		
	defines
	{
		"GLFW_INCLUDE_NONE"
	}

	filter "configurations:Debug"
		defines "FY_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "FY_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "FY_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	debugdir ("bin/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"Frosty/vendor/spdlog/include",
		"Frosty/src",
		"Frosty/vendor",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Frosty"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "FY_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "FY_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "FY_DIST"
		runtime "Release"
		optimize "on"
