project "Test"
    kind "ConsoleApp"
    language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	targetname "test_config.out"
    
    files
	{
		"src/**.h",
		"src/test_config.cpp",
	}

    includedirs
    {
        "%{wks.location}/CppServer/src",
		"%{IncludeDir.yaml_cpp}",
    }

    links        --链接库
    {
        "CppServer",
		"yaml-cpp",
    }

    filter "system:linux"
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