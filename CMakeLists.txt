# Borrows from the sc3-plugins CMakeLists.txt, credit to those authors.
cmake_minimum_required (VERSION 3.1)
project(egSC)

find_package(SuperCollider3)
if (NOT SC_FOUND)
    message(SEND_ERROR "cannot find SuperCollider3 headers. Set the variable SC_PATH.")
else()
    message(STATUS "Using SC source located at ${SC_PATH}")
endif()

add_definitions(-march=native)

include("${SC_PATH}/SCVersion.txt")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}")
message(STATUS "Building plugins for SuperCollider version: ${PROJECT_VERSION}")

add_subdirectory(source)

