//
// Created by Jonas Nilsson on 2020-11-24.
//

#include "ParameterHandler.h"
#include "Parameter.h"
#include <algorithm>

ParameterHandler::ParameterHandler(asynPortDriver *DriverPtr) : Driver(DriverPtr) {
}

void ParameterHandler::registerParameter(ParameterBase *Param) {
  asynParamType ParameterType;
  if (dynamic_cast<Parameter<std::string>*>(Param)) {
    ParameterType = asynParamOctet;
  } else {
    assert(false);
  }
  int ParameterIndex;
  Driver->createParam(Param->getParameterName().c_str(), ParameterType, &ParameterIndex);
  KnownParameters[ParameterIndex] = Param;
  Param->registerRegistrar(this);
}

void ParameterHandler::updateDbValue(ParameterBase *ParamPtr) {
  if (Driver == nullptr) {
    return;
  }
  auto FoundParameter = std::find_if(KnownParameters.begin(), KnownParameters.end(), [&](std::pair<int,ParameterBase*> Item){
    return Item.second == ParamPtr;
  });
  if (FoundParameter == KnownParameters.end()) {
    return;
  }
  auto UsedIndex = FoundParameter->first;
  if (typeid(*ParamPtr) == typeid(Parameter<std::string>)) {
    Driver->setStringParam(UsedIndex, dynamic_cast<Parameter<std::string>*>(ParamPtr)->readValue());
  } else {
    assert(false);
  }
}