# Borrows from the sc3-plugins CMakeLists.txt, credit to those authors.
cmake_minimum_required (VERSION 3.1)
project(egSC)

add_definitions(-march=native)

add_subdirectory(third_party)
add_subdirectory(src)

