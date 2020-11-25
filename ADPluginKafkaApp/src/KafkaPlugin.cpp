/** Copyright (C) 2017 European Spallation Source */

/** @file  KafkaPlugin.cpp
 *  @brief C++ implementation file for an EPICS areaDetector Kafka-plugin.
 */

#include <epicsEvent.h>
#include <epicsMessageQueue.h>
#include <epicsThread.h>
#include <epicsTime.h>
#include <iocsh.h>

#include <asynDriver.h>
#include <ciso646>
#include <epicsExport.h>
#include <string.h>

#include "KafkaPlugin.h"

static const char *driverName = "KafkaPlugin";

void KafkaPlugin::processCallbacks(NDArray *pArray) {
  // We do not need to call reserve/release as this is done by the caller when
  // in blocking mode
  // and by the thread in non-blocking mode.
  /// @todo Check the order of these calls and if all of them are needed.
  NDArrayInfo_t arrayInfo;

  NDPluginDriver::beginProcessCallbacks(pArray);

  pArray->getInfo(&arrayInfo);

  unsigned char *bufferPtr;
  size_t bufferSize;

  Serializer.SerializeData(*pArray, bufferPtr, bufferSize);
  this->unlock();
  bool addToQueueSuccess = producer.SendKafkaPacket(
      bufferPtr, bufferSize, epicsTimeToTimePoint(pArray->epicsTS));
  this->lock();
  if (not addToQueueSuccess) {
    int droppedArrays;
    getIntegerParam(NDPluginDriverDroppedArrays, &droppedArrays);
    droppedArrays++;
    setIntegerParam(NDPluginDriverDroppedArrays, droppedArrays);
  }
  callParamCallbacks();
}

asynStatus KafkaPlugin::writeOctet(asynUser *pasynUser, const char *value,
                                   size_t nChars, size_t *nActual) {
  int addr = 0;
  int function{pasynUser->reason};
  const char *functionName = "writeOctet";

  asynStatus status = getAddress(pasynUser, &addr);
  if (status != asynSuccess) {
    return (status);
  }

  if (ParamRegistrar.write<std::string>(function, {value, nChars}) or
      NDPluginDriver::writeOctet(pasynUser, value, nChars, nActual) ==
          asynSuccess) {
    /* Set the parameter in the parameter library. */
    setStringParam(addr, function, const_cast<char *>(value));
  }

  // Do callbacks so higher layers see any changes
  status = callParamCallbacks(addr, addr);

  if (status != 0) {
    epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, value=%s", driverName,
                  functionName, status, function, value);
  } else {
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, "%s:%s: function=%d, value=%s\n",
              driverName, functionName, function, value);
  }

  // We are assuming that we wrote as many characters as we received
  *nActual = nChars;
  return status;
}

asynStatus KafkaPlugin::readOctet(asynUser *pasynUser, char *value,
                                  size_t maxChars, size_t *nActual,
                                  int *eomReason) {
  int function;
  const char *paramName;
  int addr;
  epicsTimeStamp timeStamp;
  getTimeStamp(&timeStamp);
  static const char *functionName = "readOctet";

  asynStatus status = parseAsynUser(pasynUser, &function, &addr, &paramName);
  if (status != asynSuccess)
    return status;

  std::string TempString;
  if (ParamRegistrar.read<std::string>(function, TempString)) {
    strncpy(value, TempString.c_str(), TempString.size() + 1);
  } else if (NDPluginDriver::readOctet(pasynUser, value, maxChars, nActual, eomReason) == asynSuccess) {
    // Do nothing
  } else {
    status = asynError;
  }

  /* Set the timestamp */
  pasynUser->timestamp = timeStamp;
  if (status)
    epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, name=%s, value=%s",
                  driverName, functionName, status, function, paramName, value);
  else
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, name=%s, value=%s\n", driverName,
              functionName, function, paramName, value);
  if (eomReason)
    *eomReason = ASYN_EOM_END;
  *nActual = strlen(value) + 1;
  return status;
}

asynStatus KafkaPlugin::writeInt32(asynUser *pasynUser, epicsInt32 value) {
  const int function{pasynUser->reason};
  static const char *functionName = "writeInt32";

  if (ParamRegistrar.write<int32_t>(function, value) or
      NDPluginDriver::writeInt32(pasynUser, value) == asynSuccess) {
    /* Set the parameter in the parameter library. */
    setIntegerParam(function, value);
  }

  /* Do callbacks so higher layers see any changes */
  asynStatus status = callParamCallbacks();

  if (status != 0) {
    epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, value=%d", driverName,
                  functionName, status, function, value);
  } else {
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, "%s:%s: function=%d, value=%d\n",
              driverName, functionName, function, value);
  }
  return status;
}

asynStatus KafkaPlugin::readInt32(asynUser *pasynUser, epicsInt32 *value) {
  int function;
  const char *paramName;
  int addr;
  epicsTimeStamp timeStamp;
  getTimeStamp(&timeStamp);
  static const char *functionName = "readInt32";

  asynStatus status = parseAsynUser(pasynUser, &function, &addr, &paramName);
  if (status != asynSuccess)
    return status;

  if (not ParamRegistrar.read<int32_t>(function, *value) or NDPluginDriver::readInt32(pasynUser, value) != asynSuccess) {
    status = asynError;
  }

  /* Set the timestamp */
  pasynUser->timestamp = timeStamp;
  if (status)
    epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, name=%s, value=%d",
                  driverName, functionName, status, function, paramName, *value);
  else
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, name=%s, value=%d\n", driverName,
              functionName, function, paramName, *value);
  return status;
}


asynStatus KafkaPlugin::writeInt64(asynUser *pasynUser, epicsInt64 value) {
  const int function{pasynUser->reason};
  static const char *functionName = "writeInt64";

  if (ParamRegistrar.write<int64_t>(function, value) or
      NDPluginDriver::writeInt64(pasynUser, value) == asynSuccess) {
    /* Set the parameter in the parameter library. */
    setIntegerParam(function, value);
  }

  /* Do callbacks so higher layers see any changes */
  asynStatus status = callParamCallbacks();

  if (status != 0) {
    epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, value=%lli", driverName,
                  functionName, status, function, value);
  } else {
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, value=%lli\n", driverName, functionName,
              function, value);
  }
  return status;
}

asynStatus KafkaPlugin::readInt64(asynUser *pasynUser, epicsInt64 *value) {
  int function;
  const char *paramName;
  int addr;
  epicsTimeStamp timeStamp;
  getTimeStamp(&timeStamp);
  static const char *functionName = "readInt64";

  asynStatus status = parseAsynUser(pasynUser, &function, &addr, &paramName);
  if (status != asynSuccess)
    return status;

  if (not ParamRegistrar.read<int64_t>(function, *value) or NDPluginDriver::readInt64(pasynUser, value) != asynSuccess) {
    status = asynError;
  }

  /* Set the timestamp */
  pasynUser->timestamp = timeStamp;
  if (status)
    epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, name=%s, value=%lli",
                  driverName, functionName, status, function, paramName, *value);
  else
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, name=%s, value=%lli\n", driverName,
              functionName, function, paramName, *value);
  return status;
}

KafkaPlugin::KafkaPlugin(const char *portName, int queueSize,
                         int blockingCallbacks, const char *NDArrayPort,
                         int NDArrayAddr, size_t maxMemory, int priority,
                         int stackSize, const char *brokerAddress,
                         const char *brokerTopic, const char *sourceName)
    // Invoke the base class constructor
    : NDPluginDriver(portName, queueSize, blockingCallbacks, NDArrayPort,
                     NDArrayAddr, 1, 2, maxMemory, intMask, intMask, 0, 1,
                     priority, stackSize, 1),
      producer(brokerAddress, brokerTopic, &ParamRegistrar),
      Serializer(sourceName) {

  producer.StartThread();

  setStringParam(NDPluginDriverPluginType, "KafkaPlugin");
  ParamRegistrar.registerParameter(&SourceName);

  // Disable ArrayCallbacks.
  // This plugin currently does not do array callbacks, so make the setting
  // reflect the behavior
  setIntegerParam(NDArrayCallbacks, 0);

  /* Try to connect to the NDArray port */
  connectToArrayPort();
}

// Configuration routine.  Called directly, or from the iocsh function
extern "C" int KafkaPluginConfigure(const char *portName, int queueSize,
                                    int blockingCallbacks,
                                    const char *NDArrayPort, int NDArrayAddr,
                                    size_t maxMemory, const char *brokerAddress,
                                    const char *topic, const char *sourceName) {
  auto *pPlugin = new KafkaPlugin(portName, queueSize, blockingCallbacks,
                                  NDArrayPort, NDArrayAddr, maxMemory, 0, 0,
                                  brokerAddress, topic, sourceName);

  return pPlugin->start();
}

// EPICS iocsh shell commands
static const iocshArg initArg0 = {"portName", iocshArgString};
static const iocshArg initArg1 = {"frame queue size", iocshArgInt};
static const iocshArg initArg2 = {"blocking callbacks", iocshArgInt};
static const iocshArg initArg3 = {"NDArrayPort", iocshArgString};
static const iocshArg initArg4 = {"NDArrayAddr", iocshArgInt};
static const iocshArg initArg5 = {"maxMemory", iocshArgInt};
// static const iocshArg initArg6 = {"priority", iocshArgInt};
// static const iocshArg initArg7 = {"stack size", iocshArgInt};
static const iocshArg initArg8 = {"broker address", iocshArgString};
static const iocshArg initArg9 = {"topic", iocshArgString};
static const iocshArg initArg10 = {"source name", iocshArgString};

static const iocshArg *const initArgs[] = {&initArg0, &initArg1, &initArg2,
                                           &initArg3, &initArg4, &initArg5,
                                           &initArg8, &initArg9, &initArg10};
static const iocshFuncDef initFuncDef = {"KafkaPluginConfigure", 9, initArgs};
static void initCallFunc(const iocshArgBuf *args) {
  KafkaPluginConfigure(args[0].sval, args[1].ival, args[2].ival, args[3].sval,
                       args[4].ival, args[5].ival, args[6].sval, args[7].sval,
                       args[8].sval);
}

extern "C" void KafkaPluginReg(void) {
  iocshRegister(&initFuncDef, initCallFunc);
}

extern "C" {
epicsExportRegistrar(KafkaPluginReg);
}
