/** Copyright (C) 2017 European Spallation Source */

/** @file
 *  @brief A class for handling parameters.
 */

#pragma once

#include <map>
#include <asynPortDriver.h>
#include "Parameter.h"

class ParameterHandler {
public:
  ParameterHandler(asynPortDriver *DriverPtr);
  void registerParameter(ParameterBase *Param);

  template<class ParamType>
  bool write(int Index, ParamType Value) {
    try {
      auto ParamPtr = dynamic_cast<Parameter<ParamType>*>(KnownParameters.at(Index));
      if (ParamPtr == nullptr) {
        return false;
      }
      ParamPtr->writeValue(Value);
    } catch (std::out_of_range const &) {
      return false;
    }
    return true;
  }

  template <class ParamType>
  bool read(int Index, ParamType &Value) {
    try {
      auto ParamPtr = dynamic_cast<Parameter<ParamType>*>(KnownParameters.at(Index));
      if (ParamPtr == nullptr) {
        return false;
      }
      Value = ParamPtr->readValue();
    } catch (std::out_of_range const &) {
      return false;
    }
    return true;
  }
  void updateDbValue(ParameterBase *ParamPtr);
private:
  std::map<int, ParameterBase*> KnownParameters;
  asynPortDriver *Driver;
};