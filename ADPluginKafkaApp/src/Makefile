TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

INC += KafkaPlugin.h
INC += NDArraySerializer.h
INC += KafkaProducer.h
INC += TimeUtility.h
INC += Parameter.h
INC += ParameterHandler.h
INC += json/json.h
INC += json/json-forwards.h
INC += ADArray_schema_generated.h
INC += flatbuffers/base.h
INC += flatbuffers/flatbuffers.h
INC += flatbuffers/stl_emulation.h
LIBRARY_IOC += ADPluginKafka
LIB_SRCS += KafkaPlugin.cpp
LIB_SRCS += KafkaProducer.cpp
LIB_SRCS += NDArraySerializer.cpp
LIB_SRCS += jsoncpp.cpp
LIB_SRCS += TimeUtility.cpp
LIB_SRCS += Parameter.cpp
LIB_SRCS += ParameterHandler.cpp

DBD += ADPluginKafka.dbd

LIB_LIBS += NDPlugin
LIB_SYS_LIBS += rdkafka++ rdkafka


USR_CXXFLAGS += -I${AREA_DETECTOR}/ADCore/include
USR_CXXFLAGS += -std=c++11

ifneq ($(findstring static,$(EPICS_HOST_ARCH)),)
USR_CXXFLAGS_WIN32 += -DLIBRDKAFKA_STATICLIB
endif

include $(ADCORE)/ADApp/commonLibraryMakefile


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE
