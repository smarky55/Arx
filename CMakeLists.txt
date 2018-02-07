cmake_minimum_required(VERSION 3.10.2)

set(BOOST_ROOT $ENV{BOOST_ROOT})

project(arx)

include_directories(${BOOST_ROOT} include)
#include_directories($ENV{BOOST_ROOT})

# Adding source files
set(SOURCES ${SOURCES} src/ArxLoader.cpp)
set(SOURCES ${SOURCES} src/ArxPackager.cpp)

# Add header files to project
set(HEADERS ${HEADERS} include/ArxLoader.h)
set(HEADERS ${HEADERS} include/ArxPackager.h)
set(HEADERS ${HEADERS} include/ArxCommon.h)

add_library(arx ${SOURCES} ${HEADERS})

add_executable(packager src/packager.cpp)
target_link_libraries(packager arx)

install(TARGETS arx DESTINATION bin)
