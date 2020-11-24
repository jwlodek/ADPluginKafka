/** Copyright (C) 2017 European Spallation Source */

/** @file
 *  @brief A class for easier interaction with parameters.
 */

#include "Parameter.h"
#include "ParameterHandler.h"

void ParameterBase::updateDbValue() {
  if (HandlerPtr == nullptr) {return;}
  HandlerPtr->updateDbValue(this);
}

void ParameterBase::registerRegistrar(ParameterHandler *Registrar) {
  HandlerPtr = Registrar;
}