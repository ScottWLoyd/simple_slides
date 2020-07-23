workspace "SimpleSlides"
    configurations { "Debug", "Release" }    
    architecture "x64"

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
        "src/glad",
        "ext/src/glfw/include"
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

project "glfw"
    kind "StaticLib"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"

    files 
    {
        "ext/src/glfw/src/**.c",
        "ext/src/glfw/deps/**.c"
    }

    includedirs
    {
        "ext/src/glfw/include",
        "ext/src/glfw/deps"
    }