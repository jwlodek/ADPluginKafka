/** Copyright (C) 2020 European Spallation Source */

/** @file  TimeUtility.cpp
 *  @brief Some time utility code.
 */

#include "TimeUtility.h"

std::uint64_t epicsTimeToNsec(epicsTimeStamp const &Timestamp) {
  const auto TimeDiffUNIXtoEPICSepoch = 631152000L;
  const auto NSecMultiplier = 1000000000L;
  return static_cast<std::uint64_t>(Timestamp.secPastEpoch + TimeDiffUNIXtoEPICSepoch) * NSecMultiplier + Timestamp.nsec;
}

time_point epicsTimeToTimePoint(epicsTimeStamp const &TimeStamp) {
  time_point ReturnValue{std::chrono::nanoseconds(epicsTimeToNsec(TimeStamp))};
  return ReturnValue;
}