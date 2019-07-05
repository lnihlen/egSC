# Borrows from the sc3-plugins source/CMakeLists.txt and example-plugins
# CMakeLists.txt, credit to those authors.

if (NOT SC_PATH)
    message(SEND_ERROR "cannot find SuperCollider3 headers. Set the variable SC_PATH.")
else()
    message(STATUS "Using SC source located at ${SC_PATH}")
endif()

include("${SC_PATH}/SCVersion.txt")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}")
message(STATUS "Building plugins for SuperCollider version: ${PROJECT_VERSION}")

add_subdirectory(ugen)

