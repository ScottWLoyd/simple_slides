workspace "SimpleSlides"
    configurations { "Debug", "Release" }

project "SimpleSlides"
    kind "ConsoleApp"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"

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