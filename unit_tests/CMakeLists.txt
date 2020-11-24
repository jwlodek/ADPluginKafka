cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

add_definitions(-std=c++11 -pthread) #-v = verbose

find_package(GTest REQUIRED)
find_package(librdkafka REQUIRED)

if (NOT DEFINED ENV{EPICS_BASE})
    message(FATAL_ERROR "Missing environment variable \"EPICS_BASE\".")
endif()

if (NOT DEFINED ENV{EPICS_MODULES_PATH})
    message(FATAL_ERROR "Missing environment variable \"EPICS_MODULES_PATH\".")
endif()

if (APPLE)
    set(E_OS "Darwin")
    set(E_PLATFORM "darwin-x86")
    set(E_COMP "clang")
elseif(LINUX)
    message(STATUS "Assuming Linux machine running Centos 7.")
    set(E_OS "Linux")
    set(E_PLATFORM "centos7-x86_64")
    set(E_COMP "gcc")
else()
    message(FATAL_ERROR "Unknown OS. Unable to set-up the build environment.")
endif()

set(EPICS_INC_DIRS
        "$ENV{EPICS_MODULES_PATH}/asyn/include/"
        "$ENV{EPICS_MODULES_PATH}/areaDetector/ADCore/include/"
        "$ENV{EPICS_BASE}/include/compiler/${E_COMP}/"
        "$ENV{EPICS_BASE}/include/os/${E_OS}/"
        "$ENV{EPICS_BASE}/include"
        )

SET(EPICS_LIB_DIRS
        "$ENV{EPICS_MODULES_PATH}/areaDetector/ADCore/lib/${E_PLATFORM}"
        "$ENV{EPICS_MODULES_PATH}/asyn/lib/${E_PLATFORM}"
        "$ENV{EPICS_BASE}/lib/${E_PLATFORM}"
        )
add_library(epics INTERFACE)
target_include_directories(epics INTERFACE ${EPICS_INC_DIRS})
target_link_directories(epics INTERFACE ${EPICS_LIB_DIRS})
target_link_libraries(epics INTERFACE NDPlugin ADBase asyn Com)

set(Common_SRC
    jsoncpp.cpp
)

set(Common_INC
    flatbuffers/base.h
    flatbuffers/flatbuffers.h
    json/json.h
    flatbuffers/stl_emulation.h
    ADArray_schema_generated.h
    ParamUtility.h
)

list(TRANSFORM Common_SRC PREPEND "../ADPluginKafkaApp/src/")
list(TRANSFORM Common_INC PREPEND "../ADPluginKafkaApp/src/")

add_library(Common OBJECT ${Common_SRC} ${Common_INC})
target_include_directories(Common PRIVATE ../ADPluginKafkaApp/src/)

set(Plugin_SRC
  KafkaProducer.cpp
  KafkaPlugin.cpp
  NDArraySerializer.cpp
  TimeUtility.cpp
    Parameter.cpp
    ParameterHandler.cpp
)

set(Plugin_INC
  KafkaProducer.h
  KafkaPlugin.h
  NDArraySerializer.h
  TimeUtility.h
    Parameter.h
    ParameterHandler.h
)

list(TRANSFORM Plugin_SRC PREPEND "../ADPluginKafkaApp/src/")
list(TRANSFORM Plugin_INC PREPEND "../ADPluginKafkaApp/src/")

add_library(Plugin OBJECT ${Plugin_SRC} ${Plugin_INC})
target_link_libraries(Plugin PUBLIC epics librdkafka::librdkafka)
target_include_directories(Plugin PRIVATE ../ADPluginKafkaApp/src/)

set(Test_SRC
  RunTests.cpp
  GenerateNDArray.cpp
  KafkaPluginTest.cpp
  KafkaProducerTest.cpp
  NDArraySerializerTest.cpp
  NDArrayDeSerializer.cpp
  ParamUtilityTest.cpp
  PortName.cpp
  $<TARGET_OBJECTS:Plugin>
  $<TARGET_OBJECTS:Common>
)

set(Test_INC
  GenerateNDArray.h
  PortName.h
  NDArrayDeSerializer.h
)

add_executable(unit_tests ${Test_SRC} ${Test_INC})
target_include_directories(unit_tests PRIVATE "../ADPluginKafkaApp/src/")


target_link_libraries(unit_tests gtest gmock_main gmock epics Plugin)

get_filename_component(TEST_DATA_PATH "someADArray.data" DIRECTORY)
target_compile_definitions(unit_tests
    PRIVATE TEST_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DATA_PATH}/")


add_test(TestAll unit_tests)
