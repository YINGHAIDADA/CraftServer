project "CraftServer"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	-- pchheader "cnpch.h"
	-- pchsource "src/cnpch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"src",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.boost}",
	}

	links
	{
		"yaml-cpp",
		"boost"
	}

	defines
	{
		-- "_CRT_SECURE_NO_WARNINGS"
	}
	
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"CS_PLATFORM_LINUX",
		}

	filter "configurations:Debug"
		defines "CS_DEBUG"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:Release"
		defines "CS_RELEASE"
		runtime "Release"
		optimize "on"
		
	filter "configurations:Dist"
		defines "CS_DIST"
		runtime "Release"
		optimize "on"
		