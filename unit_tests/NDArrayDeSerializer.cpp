/** Copyright (C) 2017 European Spallation Source */

/** @file  NDArrayDeSerializer.cpp
 *  @brief Implementation of simple function which de-serializes EPICS NDArray
 * into a Flatbuffer.
 */

#include "NDArrayDeSerializer.h"
#include <cassert>
#include <ciso646>
#include <cstdlib>
#include <vector>

NDDataType_t GetND_DType(DType arrType) {
  switch (arrType) {
  case DType::DType_int8:
    return NDInt8;
  case DType::DType_uint8:
    return NDUInt8;
  case DType::DType_int16:
    return NDInt16;
  case DType::DType_uint16:
    return NDUInt16;
  case DType::DType_int32:
    return NDInt32;
  case DType::DType_uint32:
    return NDUInt32;
  case DType::DType_int64:
    return NDInt64;
  case DType::DType_uint64:
    return NDUInt64;
  case DType::DType_float32:
    return NDFloat32;
  case DType::DType_float64:
    return NDFloat64;
  default:
    assert(false);
  }
  return NDInt8;
}

NDAttrDataType_t GetND_AttrDType(DType attrType) {
  switch (attrType) {
  case DType::DType_int8:
    return NDAttrInt8;
  case DType::DType_uint8:
    return NDAttrUInt8;
  case DType::DType_int16:
    return NDAttrInt16;
  case DType::DType_uint16:
    return NDAttrUInt16;
  case DType::DType_int32:
    return NDAttrInt32;
  case DType::DType_uint32:
    return NDAttrUInt32;
  case DType::DType_int64:
    return NDAttrInt64;
  case DType::DType_uint64:
    return NDAttrUInt64;
  case DType::DType_float32:
    return NDAttrFloat32;
  case DType::DType_float64:
    return NDAttrFloat64;
  case DType::DType_c_string:
    return NDAttrString;
  default:
    assert(false);
  }
  return NDAttrInt8;
}

size_t GetTypeSize(DType attrType) {
  switch (attrType) {
  case DType::DType_int8:
    return 1;
  case DType::DType_uint8:
    return 1;
  case DType::DType_int16:
    return 2;
  case DType::DType_uint16:
    return 2;
  case DType::DType_int32:
    return 4;
  case DType::DType_uint32:
    return 4;
  case DType::DType_int64:
    return 8;
  case DType::DType_uint64:
    return 8;
  case DType::DType_float32:
    return 4;
  case DType::DType_float64:
    return 8;
  case DType::DType_c_string:
    return 1;
  default:
    assert(false);
  }
  return 1;
}

epicsTimeStamp nSecToEpicsTimestamp(std::uint64_t nSec) {
  const auto TimeDiffUNIXtoEPICSepoch = 631152000L;
  const auto NSecMultiplier = 1000000000L;
  return epicsTimeStamp{static_cast<epicsUInt32>(nSec / NSecMultiplier - TimeDiffUNIXtoEPICSepoch), static_cast<epicsUInt32>(nSec % NSecMultiplier)};
}

void DeSerializeData(NDArrayPool *pNDArrayPool, const unsigned char *bufferPtr,
                     NDArray *&pArray) {
  auto recvArr = GetADArray(bufferPtr);
  int id = recvArr->id();
  std::uint64_t Timestamp = recvArr->timestamp();
  std::vector<size_t> dims(recvArr->dimensions()->begin(), recvArr->dimensions()->end());
  NDDataType_t dataType = GetND_DType(recvArr->data_type());
  const void *pData = reinterpret_cast<const void *>(recvArr->data()->Data());
  int pData_size = recvArr->data()->size();

  pArray = pNDArrayPool->alloc(static_cast<int>(dims.size()), dims.data(),
                               dataType, 0, nullptr);

  NDAttributeList *attrPtr = pArray->pAttributeList;
  attrPtr->clear();
  for (int i = 0; i < recvArr->attributes()->size(); i++) {
    auto cAttr = recvArr->attributes()->Get(i);
    attrPtr->add(
        new NDAttribute(cAttr->name()->c_str(), cAttr->description()->c_str(),
                        NDAttrSourceDriver, cAttr->source()->c_str(),
                        GetND_AttrDType(cAttr->data_type()),
                        reinterpret_cast<void *>(const_cast<std::uint8_t *>(
                            cAttr->data()->Data()))));
  }

  std::memcpy(pArray->pData, pData, pData_size);

  pArray->uniqueId = id;
  pArray->timeStamp = Timestamp / 1e9 - 631152000L;
  pArray->epicsTS = nSecToEpicsTimestamp(Timestamp);
}
