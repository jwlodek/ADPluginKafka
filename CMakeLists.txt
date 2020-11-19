cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project("ADPluginKafka")
set(ADPluginKafka_VERSION_MAJOR 1)
set(ADPluginKafka_VERSION_MINOR 0)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

set(CONAN_PROFILE "default" CACHE STRING "Name of conan profile to use, uses default by default")
set(CONAN_FILE "conanfile.txt" CACHE STRING "Name of the conanfile to use, must be placed in the project's conan directory")
set(CONAN "AUTO" CACHE STRING "conan options AUTO (conan must be in path), MANUAL (expects conanbuildinfo.cmake in build directory) or DISABLE")

if(${CONAN} MATCHES "AUTO")
    include(${PROJECT_SOURCE_DIR}/cmake_modules/conan.cmake)
    conan_cmake_run(CONANFILE ${CONAN_FILE}
            PROFILE ${CONAN_PROFILE}
            BASIC_SETUP ${OUTPUT_DIRS}
            BUILD_TYPE "None"
            BUILD outdated
            KEEP_RPATHS)
elseif(${CONAN} MATCHES "MANUAL")
    if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        conan_basic_setup(${OUTPUT_DIRS} KEEP_RPATHS)
    else()
        message(FATAL_ERROR "CONAN set to MANUAL but no file named conanbuildinfo.cmake found in build directory")
    endif()
elseif(${CONAN} MATCHES "DISABLE")
    message(WARNING "Conan is disabled so you will need to put your own cmake find modules in the project's \"cmake\" directory")
else()
    message(FATAL_ERROR "Unrecognised option for CONAN, use AUTO, MANUAL or DISABLE")
endif()

add_subdirectory(unit_tests)
