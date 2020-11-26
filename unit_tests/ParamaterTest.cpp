/** Copyright (C) 2017 European Spallation Source */

/** @file  main.cpp
 *  @brief Test the parameter class.
 */

#include "Parameter.h"
#include "ParameterHandler.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class ParameterHandlerStandIn : public ParameterHandler {
public:
  ParameterHandlerStandIn() : ParameterHandler(nullptr) {};
  MOCK_METHOD(void, updateDbValue, (ParameterBase*), (override));
};

TEST(Parameter, CheckInitName) {
  std::string TestName{"some test name"};
  ParameterBase UnderTest(TestName);
  EXPECT_EQ(TestName, UnderTest.getParameterName());
}

using ::testing::Exactly;

TEST(Parameter, UpdateDbValue) {
  ParameterHandlerStandIn PHandler;
  ParameterBase UnderTest("some_name");
  UnderTest.registerRegistrar(&PHandler);
  EXPECT_CALL(PHandler, updateDbValue(&UnderTest)).Times(Exactly(1));
  UnderTest.updateDbValue();
}

TEST(Parameter, FunctionCalls) {
  ParameterHandlerStandIn PHandler;
  int ReadCalls{0};
  int WriteCalls{0};
  int LatestWriteValue{-1};
  int WriteValue{987654321};
  const int TestReturnValue{123456789};
  auto DoRead = [&]() -> int {
    ReadCalls++;
    return TestReturnValue;
  };
  auto DoWrite = [&](int NewValue) -> bool {
    WriteCalls++;
    LatestWriteValue = NewValue;
    return true;
  };
  Parameter<int> UnderTest("some_name", DoWrite, DoRead);
  EXPECT_EQ(UnderTest.readValue(), TestReturnValue);
  EXPECT_EQ(ReadCalls, 1);

  UnderTest.writeValue(WriteValue);
  EXPECT_EQ(WriteCalls, 1);
  EXPECT_EQ(LatestWriteValue, WriteValue);
}