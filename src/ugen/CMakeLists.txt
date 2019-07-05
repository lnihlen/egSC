# Borrows from the sc3-plugins source/CMakeLists.txt and example-plugins
# CMakeLists.txt, credit to those authors.

set(egSCUGen_files
    DuffingOsc.cpp
    SharpFineRKNG8.hpp
)

add_library(egSCUGen MODULE ${egSCUGen_files})

target_include_directories(egSCUGen PRIVATE ${SC_PATH}/include/plugin_interface)
target_include_directories(egSCUGen PRIVATE ${SC_PATH}/include/common)
target_include_directories(egSCUGen PRIVATE ${SC_PATH}/common)

target_include_directories(egSCUGen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS egSCUGen DESTINATION "lib/SuperCollider/plugins")

