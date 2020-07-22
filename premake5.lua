workspace "SimpleSlides"
    configurations { "Debug", "Release" }    
    architecture "x64"

include "ext/src/SDL2/SDL2.lua"

project "SimpleSlides"
    kind "ConsoleApp"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"

    files 
    {
        "src/**.h",
        "src/**.c"
    }

    includedirs 
    {
        "src",
        "ext/src/SDL2/include"
    }

    links 
    {
        "SDL2"
    }

    filter "configurations:Debug"
        defines
        {
            "DEBUG"
        }
        symbols "On"

    filter "configurations:Release"
        defines
        {
            "NDEBUG"
        }
        optimize "On"

    filter "system:Windows"
        defines
        {
            "PLATFORM_WINDOWS" 
		  }