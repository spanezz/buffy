cmake_minimum_required(VERSION 3.13.4)

include(GNUInstallDirs)

project(buffy VERSION 2.0 LANGUAGES CXX)

find_package(PkgConfig)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(ZLIB REQUIRED zlib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_compile_options(-fdiagnostics-color=always -Wformat=2 -Wall -Wextra -Wno-unused-parameter)

file(GLOB BUFFY_SOURCES *.cpp buffy/*.cc buffy/config/*.cc buffy/mailfolder/*.cc buffy/utils/*.cc)
list(FILTER BUFFY_SOURCES EXCLUDE REGEX -tut\.cc$)
list(FILTER BUFFY_SOURCES EXCLUDE REGEX test.+\.cc$)

add_executable(buffy ${BUFFY_SOURCES})

qt5_use_modules(buffy Core Gui Widgets)

target_include_directories(buffy PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(buffy ${GLIB_LIBRARIES} ${ZLIB_LIBRARIES})
target_include_directories(buffy PUBLIC ${GLIB_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
target_compile_options(buffy PUBLIC ${GLIB_CFLAGS_OTHER} ${ZLIB_CFLAGS_OTHER})

