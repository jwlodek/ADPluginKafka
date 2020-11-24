/** Copyright (C) 2017 European Spallation Source */

/** @file  NDArraySerializer.cpp
 *  @brief Implementation of simple class which serializes EPICS NDArray into a
 * Flatbuffer.
 */

#include "NDArraySerializer.h"
#include <cassert>
#include <ciso646>
#include <memory>
#include <vector>
#include <cstdint>
#include "TimeUtility.h"

NDArraySerializer::NDArraySerializer(std::string SourceName, const flatbuffers::uoffset_t bufferSize)
    : SourceName(SourceName), builder(bufferSize) {}

bool NDArraySerializer::setSourceName(std::string NewSourceName) {
  if (NewSourceName.empty()) {
    return false;
  }
  SourceName = std::move(NewSourceName);
  return true;
}

std::string NDArraySerializer::getSourceName() {
  return SourceName;
}

void NDArraySerializer::SerializeData(NDArray &pArray,
                                      unsigned char *&bufferPtr,
                                      size_t &bufferSize) {
  NDArrayInfo ndInfo{};
  pArray.getInfo(&ndInfo);

  // Required to not have a memory leak
  builder.Clear();

  auto SourceNamePtr = builder.CreateString(SourceName);

  std::vector<std::uint64_t> tempDims;
  for (size_t y = 0; y < pArray.ndims; y++) {
    tempDims.push_back(pArray.dims[y].size);
  }
  auto dims = builder.CreateVector(tempDims);
  auto dType = GetFB_DType(pArray.dataType);

  std::uint8_t *tempPtr;
  auto payload =
      builder.CreateUninitializedVector(ndInfo.totalBytes, 1, &tempPtr);
  std::memcpy(tempPtr, pArray.pData, ndInfo.totalBytes);

  // Get all attributes of this data package
  std::vector<flatbuffers::Offset<Attribute>> attrVec;

  // When passing NULL, get first element
  NDAttribute *attr_ptr = pArray.pAttributeList->next(nullptr);

  // Itterate over attributes, next(ptr) returns NULL when there are no more
  while (attr_ptr != nullptr) {
    auto temp_attr_str = builder.CreateString(attr_ptr->getName());
    auto temp_attr_desc = builder.CreateString(attr_ptr->getDescription());
    auto temp_attr_src = builder.CreateString(attr_ptr->getSource());
    size_t bytes;
    NDAttrDataType_t c_type;
    attr_ptr->getValueInfo(&c_type, &bytes);
    auto attrDType = GetFB_DType(c_type);

    std::unique_ptr<char[]> attrValueBuffer(new char[bytes]);
    int attrValueRes = attr_ptr->getValue(
        c_type, reinterpret_cast<void *>(attrValueBuffer.get()), bytes);
    if (ND_SUCCESS == attrValueRes) {
      auto attrValuePayload = builder.CreateVector(
          reinterpret_cast<unsigned char *>(attrValueBuffer.get()), bytes);

      auto attr = CreateAttribute(builder, temp_attr_str,
                                               temp_attr_desc, temp_attr_src,
                                               attrDType, attrValuePayload);
      attrVec.push_back(attr);
    } else {
      assert(false);
    }

    attr_ptr = pArray.pAttributeList->next(attr_ptr);
  }
  auto attributes = builder.CreateVector(attrVec);
  auto Timestamp = epicsTimeToNsec(pArray.epicsTS);
  auto kf_pkg =
      CreateADArray(builder, SourceNamePtr, pArray.uniqueId, Timestamp, dims, dType, payload, attributes);

  // Write data to buffer
  builder.Finish(kf_pkg, ADArrayIdentifier());

  bufferPtr = builder.GetBufferPointer();
  bufferSize = builder.GetSize();
}

DType NDArraySerializer::GetFB_DType(NDDataType_t arrType) {
  switch (arrType) {
  case NDInt8:
    return DType::DType_int8;
  case NDUInt8:
    return DType::DType_uint8;
  case NDInt16:
    return DType::DType_int16;
  case NDUInt16:
    return DType::DType_uint16;
  case NDInt32:
    return DType::DType_int32;
  case NDUInt32:
    return DType::DType_uint32;
  case NDInt64:
    return DType::DType_int64;
  case NDUInt64:
    return DType::DType_uint64;
  case NDFloat32:
    return DType::DType_float32;
  case NDFloat64:
    return DType::DType_float64;
  default:
    assert(false);
  }
  return DType::DType_int8;
}

NDDataType_t NDArraySerializer::GetND_DType(DType arrType) {
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

DType NDArraySerializer::GetFB_DType(NDAttrDataType_t attrType) {
  switch (attrType) {
  case NDAttrInt8:
    return DType::DType_int8;
  case NDAttrUInt8:
    return DType::DType_uint8;
  case NDAttrInt16:
    return DType::DType_int16;
  case NDAttrUInt16:
    return DType::DType_uint16;
  case NDAttrInt32:
    return DType::DType_int32;
  case NDAttrUInt32:
    return DType::DType_uint32;
  case NDAttrInt64:
    return DType::DType_int64;
  case NDAttrUInt64:
    return DType::DType_uint64;
  case NDAttrFloat32:
    return DType::DType_float32;
  case NDAttrFloat64:
    return DType::DType_float64;
  case NDAttrString:
    return DType::DType_c_string;
  default:
    assert(false);
  }
  return DType::DType_int8;
}

NDAttrDataType_t NDArraySerializer::GetND_AttrDType(DType attrType) {
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
