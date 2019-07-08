# Borrows from the sc3-plugins source/CMakeLists.txt and example-plugins
# CMakeLists.txt, credit to those authors.

set(egSCUGen_files
    Duffing.cpp
    LinearIntegrator.hpp
    SharpFineRKNG8.hpp
)

add_library(egSCUGen MODULE ${egSCUGen_files})

target_include_directories(egSCUGen PRIVATE ${SC_PATH}/include/plugin_interface)
target_include_directories(egSCUGen PRIVATE ${SC_PATH}/include/common)
target_include_directories(egSCUGen PRIVATE ${SC_PATH}/common)

target_include_directories(egSCUGen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS egSCUGen DESTINATION "lib/SuperCollider/plugins")

set(egSCUGen_test_files
    SharpFineRKNG8.hpp
    SharpFineRKNG8_test.cpp
    test_ugen.cpp
)

add_executable(test_ugen ${egSCUGen_test_files})
target_include_directories(test_ugen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(test_ugen PRIVATE ${DOCTEST_INCLUDE_DIR})
target_link_libraries(test_ugen doctest)

