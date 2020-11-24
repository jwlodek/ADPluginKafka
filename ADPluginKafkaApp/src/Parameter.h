/** Copyright (C) 2017 European Spallation Source */

/** @file
 *  @brief A class for easier interaction with parameters.
 */

#pragma once

#include <string>
#include <functional>

class ParameterHandler;

class ParameterBase {
public:
  ParameterBase(std::string Name) : ParameterName(Name) {}
  virtual ~ParameterBase() = default;
  void registerRegistrar(ParameterHandler *Registrar);
  std::string getParameterName() {return ParameterName;}
  void setHandlerPointer(ParameterHandler *NewPtr);
  void updateDbValue();
private:
  ParameterHandler *HandlerPtr{nullptr};
  std::string ParameterName;
};

template <class ParamType>
class Parameter : public ParameterBase{
public:
  Parameter(std::string Name, std::function<bool(ParamType)> WriteParamFunc, std::function<ParamType()> ReadParamFunc) : ParameterBase(Name), WriteFunc(WriteParamFunc), ReadFunc(ReadParamFunc) {}
  void writeValue(ParamType NewValue) {
    WriteFunc(NewValue);
  }
  ParamType readValue() {
    return ReadFunc();
  }
private:
  std::function<bool(ParamType)> WriteFunc;
  std::function<ParamType()> ReadFunc;
};