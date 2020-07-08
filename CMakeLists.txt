cmake_minimum_required(VERSION 3.13.4)

include(GNUInstallDirs)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

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

# Testable library code
file(GLOB LIBBUFFY_SOURCES buffy/*.cc buffy/mailfolder/*.cc buffy/utils/*.cc)
list(FILTER LIBBUFFY_SOURCES EXCLUDE REGEX -tut\.cc$)
list(FILTER LIBBUFFY_SOURCES EXCLUDE REGEX -test\.cc$)
list(FILTER LIBBUFFY_SOURCES EXCLUDE REGEX test.+\.cc$)
add_library(libbuffy OBJECT ${LIBBUFFY_SOURCES})
target_include_directories(libbuffy PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(libbuffy ${GLIB_LIBRARIES} ${ZLIB_LIBRARIES})
target_include_directories(libbuffy PUBLIC ${GLIB_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
target_compile_options(libbuffy PUBLIC ${GLIB_CFLAGS_OTHER} ${ZLIB_CFLAGS_OTHER})

# buffy program
file(GLOB BUFFY_SOURCES *.cpp resources.qrc)
add_executable(buffy ${BUFFY_SOURCES})
qt5_use_modules(buffy Core Gui Widgets)
target_include_directories(buffy PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(buffy ${GLIB_LIBRARIES} ${ZLIB_LIBRARIES} libbuffy)
target_include_directories(buffy PUBLIC ${GLIB_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
target_compile_options(buffy PUBLIC ${GLIB_CFLAGS_OTHER} ${ZLIB_CFLAGS_OTHER})
install(TARGETS buffy RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Test suite
enable_testing()
file(GLOB TEST_SOURCES buffy/*-test.cc buffy/mailfolder/*-test.cc buffy/utils/test*.cc)
add_executable(test-all ${TEST_SOURCES})
add_test(NAME test-all COMMAND test-all)
add_definitions(-DTEST_DATA_DIR="${CMAKE_SOURCE_DIR}/testdata")
target_include_directories(test-all PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(test-all ${GLIB_LIBRARIES} ${ZLIB_LIBRARIES} libbuffy)
target_include_directories(test-all PUBLIC ${GLIB_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
target_compile_options(test-all PUBLIC ${GLIB_CFLAGS_OTHER} ${ZLIB_CFLAGS_OTHER})
