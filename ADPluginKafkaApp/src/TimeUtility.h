/** Copyright (C) 2020 European Spallation Source */

/** @file  TimeUtility.h
 *  @brief Some time utility code.
 */

#pragma once

#include <chrono>
#include <epicsTime.h>

using time_point = std::chrono::high_resolution_clock::time_point;

std::uint64_t epicsTimeToNsec(epicsTimeStamp const &Timestamp);

time_point epicsTimeToTimePoint(epicsTimeStamp const &TimeStamp);