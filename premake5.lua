workspace "CppServer"
	architecture "x86_64"
	startproject "Test"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
		
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["yaml_cpp"] = "/usr/local/include/yaml-cpp"
IncludeDir["boost"] = "/usr/include/boost"

include "CppServer"
include "Test"

