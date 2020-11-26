/** Copyright (C) 2017 European Spallation Source */

/** @file
 *  @brief A stand in for use in unit tests.
 */

#include "NDPluginDriverStandIn.h"

std::unique_ptr<NDPluginDriverStandIn> createStandInDriverPlugin() {
  static int NameCtr{0};
  std::string portName("someNameFirst" + std::to_string(NameCtr++));
  int queueSize = 10;
  int blockingCallbacks = 0;
  std::string NDArrayPort("NDArrayPortName");
  int NDArrayAddr = 42;
  int numberOfParams = 0;
  size_t maxMemory = 10;
  int mask1 = asynInt8ArrayMask | asynInt16ArrayMask | asynInt32ArrayMask |
      asynFloat32ArrayMask | asynFloat64ArrayMask;
  int mask2 = asynInt8ArrayMask | asynInt16ArrayMask | asynInt32ArrayMask |
      asynFloat32ArrayMask | asynFloat64ArrayMask;
  int priority = 0;
  int stackSize = 5;
  return std::unique_ptr<NDPluginDriverStandIn>(new NDPluginDriverStandIn(
      portName.c_str(), queueSize, blockingCallbacks, NDArrayPort.c_str(),
      NDArrayAddr, 1, numberOfParams, 2, maxMemory, mask1, mask2, 0, 1,
      priority, stackSize));
}