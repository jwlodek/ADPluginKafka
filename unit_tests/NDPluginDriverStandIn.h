/** Copyright (C) 2017 European Spallation Source */

/** @file
 *  @brief A stand in for use in unit tests.
 */

#pragma once

#include <NDPluginDriver.h>
#include <gmock/gmock.h>
#include <memory>


/// @brief Simple stand-in class used for unit tests.
class NDPluginDriverStandIn : public NDPluginDriver {
public:
  NDPluginDriverStandIn(const char *portName, int queueSize,
                        int blockingCallbacks, const char *NDArrayPort,
                        int NDArrayAddr, int maxAddr, int numParams,
                        int maxBuffers, size_t maxMemory, int interfaceMask,
                        int interruptMask, int asynFlags, int autoConnect,
                        int priority, int stackSize)
      : NDPluginDriver(portName, queueSize, blockingCallbacks, NDArrayPort,
                       NDArrayAddr, maxAddr, numParams, maxBuffers, maxMemory,
                       interfaceMask, interruptMask, asynFlags, autoConnect,
                       priority, stackSize){};
  MOCK_METHOD(asynStatus, setStringParam, (int, const char *), (override));
  MOCK_METHOD(asynStatus, setStringParam, (int, const std::string& ), (override));
  MOCK_METHOD(asynStatus, setIntegerParam, (int, int), (override));
  MOCK_METHOD(asynStatus, setInteger64Param, (int, int64_t), (override));
  MOCK_METHOD(asynStatus, createParam, (const char *, asynParamType, int *), (override));
  MOCK_METHOD(void, processCallbacks, (NDArray*), (override));
};

std::unique_ptr<NDPluginDriverStandIn> createStandInDriverPlugin();