cmake_minimum_required(VERSION 3.10.2)

project(arx)

include_directories(include)

# Adding source files
set(SOURCES ${SOURCES} src/ArxLoader.cpp)

add_library(arx ${SOURCES})

install(TARGETS arx DESTINATION bin)
