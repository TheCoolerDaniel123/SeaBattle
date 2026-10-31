cmake_minimum_required(VERSION 3.13)
project(SeaBattle VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SEABATTLE_DEBUG "Enable debug commands (e.g. 'showme' reveals the bot's ships)" OFF)
option(SEABATTLE_BUILD_TESTS "Build the unit tests" ON)

add_library(seabattle_core STATIC
    src/AIField.cpp
    src/BotPlayer.cpp
    src/Command.cpp
    src/Console.cpp
    src/Coord.cpp
    src/Field.cpp
    src/Game.cpp
    src/PlayerField.cpp
    src/Ship.cpp)
target_include_directories(seabattle_core PUBLIC include)

if(MSVC)
    target_compile_options(seabattle_core PRIVATE /W4)
else()
    target_compile_options(seabattle_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(SEABATTLE_DEBUG)
    target_compile_definitions(seabattle_core PUBLIC SEABATTLE_DEBUG)
endif()

add_executable(seabattle src/main.cpp)
target_link_libraries(seabattle PRIVATE seabattle_core)
if(MINGW)
    # Link the MinGW runtime statically: no libgcc/libstdc++/libwinpthread DLLs next to the exe.
    target_link_options(seabattle PRIVATE -static)
endif()

if(SEABATTLE_BUILD_TESTS)
    enable_testing()
    add_executable(seabattle_tests tests/tests.cpp)
    target_link_libraries(seabattle_tests PRIVATE seabattle_core)
    add_test(NAME seabattle_tests COMMAND seabattle_tests)
endif()
