/** Copyright (C) 2017 European Spallation Source */

/** @file  main.cpp
 *  @brief Test the parameter handler.
 */

#include "Parameter.h"
#include "ParameterHandler.h"
#include <gtest/gtest.h>
#include "NDPluginDriverStandIn.h"

using ::testing::_;
using ::testing::Exactly;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::SetArgPointee;
using ::testing::DoAll;
using ::testing::An;

TEST(ParameterHandler, RegisterStringParameter) {
  std::string ParameterName{"PARAM_NAME"};
  Parameter<std::string> Parameter(ParameterName, [](std::string){return true;}, []()->std::string {return {};});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(StrEq(ParameterName), asynParamOctet, _)).Times(Exactly(1)).WillOnce(Return(asynSuccess));
  UnderTest.registerParameter(&Parameter);
}

TEST(ParameterHandler, RegisterInt32Parameter) {
  std::string ParameterName{"PARAM_NAME"};
  Parameter<int32_t> Parameter(ParameterName, [](int32_t){return true;}, []()->int32_t {return {};});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(StrEq(ParameterName), asynParamInt32, _)).Times(Exactly(1)).WillOnce(Return(asynSuccess));
  UnderTest.registerParameter(&Parameter);
}

TEST(ParameterHandler, RegisterInt64Parameter) {
  std::string ParameterName{"PARAM_NAME"};
  Parameter<int64_t> Parameter(ParameterName, [](int64_t){return true;}, []()->int64_t {return {};});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(StrEq(ParameterName), asynParamInt64, _)).Times(Exactly(1)).WillOnce(Return(asynSuccess));
  UnderTest.registerParameter(&Parameter);
}

TEST(ParameterHandler, RegisterUnknownTypeParameter) {
  std::string ParameterName{"PARAM_NAME"};
  Parameter<uint32_t> Parameter(ParameterName, [](uint32_t){return true;}, []()->uint32_t {return {};});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(0);
  EXPECT_THROW(UnderTest.registerParameter(&Parameter), std::out_of_range);
}

TEST(ParameterHandler, WriteValueSuccess) {
  int32_t WrittenValue{0};
  int32_t const WriteValue{42};
  int32_t const ParamIndex{12345};
  Parameter<int32_t> Parameter("PARAM_NAME", [&](int32_t Val){WrittenValue = Val; return true;}, []()->int32_t {return {};});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(Exactly(1)).WillOnce(DoAll(SetArgPointee<2>(ParamIndex), Return(asynSuccess)));
  UnderTest.registerParameter(&Parameter);
  EXPECT_TRUE(UnderTest.write(ParamIndex, WriteValue));
  EXPECT_EQ(WriteValue, WrittenValue);
}

TEST(ParameterHandler, WriteValueReturnFalse) {
  int32_t const ParamIndex{12345};
  Parameter<int32_t> Parameter("PARAM_NAME", [&](int32_t){return false;}, []()->int32_t {return {};});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(Exactly(1)).WillOnce(DoAll(SetArgPointee<2>(ParamIndex), Return(asynSuccess)));
  UnderTest.registerParameter(&Parameter);
  EXPECT_FALSE(UnderTest.write(ParamIndex, 0));
}

TEST(ParameterHandler, WriteValueWrongIndex) {
  int32_t WrittenValue{0};
  int32_t const WriteValue{42};
  int32_t const ParamIndex{12345};
  Parameter<int32_t> Parameter("PARAM_NAME", [&](int32_t Val){WrittenValue = Val; return true;}, []()->int32_t {return {};});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(Exactly(1)).WillOnce(DoAll(SetArgPointee<2>(ParamIndex), Return(asynSuccess)));
  UnderTest.registerParameter(&Parameter);
  EXPECT_FALSE(UnderTest.write(ParamIndex + 1, WriteValue));
  EXPECT_EQ(0, WrittenValue);
}

TEST(ParameterHandler, WriteValueWrongType) {
  int32_t WrittenValue{0};
  double const WriteValue{42};
  int32_t const ParamIndex{12345};
  Parameter<int32_t> Parameter("PARAM_NAME", [&](int32_t Val){WrittenValue = Val; return true;}, []()->int32_t {return {};});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(Exactly(1)).WillOnce(DoAll(SetArgPointee<2>(ParamIndex), Return(asynSuccess)));
  UnderTest.registerParameter(&Parameter);
  EXPECT_FALSE(UnderTest.write(ParamIndex, WriteValue));
  EXPECT_EQ(0, WrittenValue);
}

TEST(ParameterHandler, ReadValueSuccess) {
  int32_t const ReturnValue{42};
  int32_t const ParamIndex{12345};
  Parameter<int32_t> Parameter("PARAM_NAME", [&](int32_t){return true;}, []()->int32_t {return ReturnValue;});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(Exactly(1)).WillOnce(DoAll(SetArgPointee<2>(ParamIndex), Return(asynSuccess)));
  UnderTest.registerParameter(&Parameter);
  int32_t TempVal{0};
  EXPECT_TRUE(UnderTest.read(ParamIndex, TempVal));
  EXPECT_EQ(TempVal, ReturnValue);
}

TEST(ParameterHandler, ReadValueWrongIndex) {
  int32_t const ReturnValue{42};
  int32_t const ParamIndex{12345};
  Parameter<int32_t> Parameter("PARAM_NAME", [&](int32_t){return true;}, []()->int32_t {return ReturnValue;});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(Exactly(1)).WillOnce(DoAll(SetArgPointee<2>(ParamIndex), Return(asynSuccess)));
  UnderTest.registerParameter(&Parameter);
  int32_t TempVal{0};
  EXPECT_FALSE(UnderTest.read(ParamIndex + 1, TempVal));
  EXPECT_EQ(TempVal, 0);
}

TEST(ParameterHandler, ReadValueWrongType) {
  int32_t const ReturnValue{42};
  int32_t const ParamIndex{12345};
  Parameter<int32_t> Parameter("PARAM_NAME", [&](int32_t){return true;}, []()->int32_t {return ReturnValue;});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(Exactly(1)).WillOnce(DoAll(SetArgPointee<2>(ParamIndex), Return(asynSuccess)));
  UnderTest.registerParameter(&Parameter);
  std::string TempVal;
  EXPECT_FALSE(UnderTest.read(ParamIndex, TempVal));
  EXPECT_TRUE(TempVal.empty());
}

TEST(ParameterHandler, UpdateDbUnknownParameter) {
  int32_t const ReturnValue{42};
  int32_t const ParamIndex{12345};
  Parameter<int32_t> Parameter1("PARAM_NAME", [&](int32_t){return true;}, []()->int32_t {return ReturnValue;});
  Parameter<int32_t> Parameter2("PARAM_NAME2", [&](int32_t){return true;}, []()->int32_t {return ReturnValue;});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(Exactly(1)).WillOnce(DoAll(SetArgPointee<2>(ParamIndex), Return(asynSuccess)));
  UnderTest.registerParameter(&Parameter1);
  EXPECT_CALL(*DriverPlugin, setStringParam(_, An<const std::string&>())).Times(0);
  EXPECT_CALL(*DriverPlugin, setInteger64Param(_, _)).Times(0);
  EXPECT_CALL(*DriverPlugin, setIntegerParam(_, _)).Times(0);
  UnderTest.updateDbValue(&Parameter2);
}

TEST(ParameterHandler, UpdateDbInt32Parameter) {
  int32_t const ReturnValue{42};
  int32_t const ParamIndex{12345};
  Parameter<int32_t> Parameter1("PARAM_NAME", [&](int32_t){return true;}, []()->int32_t {return ReturnValue;});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(Exactly(1)).WillOnce(DoAll(SetArgPointee<2>(ParamIndex), Return(asynSuccess)));
  UnderTest.registerParameter(&Parameter1);
  EXPECT_CALL(*DriverPlugin, setIntegerParam(ParamIndex, ReturnValue)).Times(Exactly(1)).WillOnce(Return(asynSuccess));
  UnderTest.updateDbValue(&Parameter1);
}

TEST(ParameterHandler, UpdateDbInt64Parameter) {
  int64_t const ReturnValue{42};
  int64_t const ParamIndex{12345};
  Parameter<int64_t> Parameter1("PARAM_NAME", [&](int64_t){return true;}, []()->int64_t {return ReturnValue;});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(Exactly(1)).WillOnce(DoAll(SetArgPointee<2>(ParamIndex), Return(asynSuccess)));
  UnderTest.registerParameter(&Parameter1);
  EXPECT_CALL(*DriverPlugin, setInteger64Param(ParamIndex, ReturnValue)).Times(Exactly(1)).WillOnce(Return(asynSuccess));
  UnderTest.updateDbValue(&Parameter1);
}

TEST(ParameterHandler, UpdateDbStringParameter) {
  std::string const ReturnValue{"test_string"};
  int64_t const ParamIndex{12345};
  Parameter<std::string> Parameter1("PARAM_NAME", [&](std::string){return true;}, [&]()->std::string {return ReturnValue;});
  auto DriverPlugin = createStandInDriverPlugin();
  ParameterHandler UnderTest(DriverPlugin.get());
  EXPECT_CALL(*DriverPlugin, createParam(_, _, _)).Times(Exactly(1)).WillOnce(DoAll(SetArgPointee<2>(ParamIndex), Return(asynSuccess)));
  UnderTest.registerParameter(&Parameter1);
  EXPECT_CALL(*DriverPlugin, setStringParam(ParamIndex, ReturnValue)).Times(Exactly(1)).WillOnce(Return(asynSuccess));
  UnderTest.updateDbValue(&Parameter1);
}