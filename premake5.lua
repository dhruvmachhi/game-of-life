workspace "GameOfLife"
    configurations { "Debug", "Release"}
    platforms { "Unix", "Windows" }
    location "build"

project "GameOfLife"
    kind "ConsoleApp"
    language "C"
    targetdir "build"

    files { "src/*.h", "src/*.c" }

    filter "platforms:Unix"
        links "curses"

    filter "platforms:Windows"
        links "pdcurses" -- idk if this is gonna work

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        optimize "On"