workspace "SimpleSlides"
    configurations { "Debug", "Release" }
    platforms { "x64" }

project "SimpleSlides"
    kind "ConsoleApp"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"

    require "ext/src/SDL2"

    files 
    {
        "src/**.h",
        "src/**.c"
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

    filter "platforms:x64"
        architecture "x86_64"