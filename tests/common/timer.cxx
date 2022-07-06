// This file is part of the rendu Project. See AUTHORS file for Copyright information
// Created by Boil on 2022/7/7.
//
#define CATCH_CONFIG_ENABLE_CHRONO_STRINGMAKER
#include "rd_catch2.h"

#include "timer.h"

TEST_CASE("TimeTracker: Check if time passed")
{
  TimeTracker tracker(1000 /*ms*/);
  REQUIRE_FALSE(tracker.Passed());
  REQUIRE(tracker.GetExpiry() == 1s);

  tracker.Update(500 /*ms*/);
  REQUIRE_FALSE(tracker.Passed());
  REQUIRE(tracker.GetExpiry() == 500ms);

  tracker.Update(500 /*ms*/);
  REQUIRE(tracker.Passed());
  REQUIRE(tracker.GetExpiry() == 0s);

  tracker.Update(500 /*ms*/);
  REQUIRE(tracker.Passed());
  REQUIRE(tracker.GetExpiry() == -500ms);
}

TEST_CASE("TimeTracker: Reset timer")
{
  TimeTracker tracker(1000 /*ms*/);
  REQUIRE_FALSE(tracker.Passed());
  REQUIRE(tracker.GetExpiry() == 1s);

  tracker.Update(1000 /*ms*/);
  REQUIRE(tracker.Passed());
  REQUIRE(tracker.GetExpiry() == 0s);

  tracker.Reset(1000 /*ms*/);
  REQUIRE_FALSE(tracker.Passed());
  REQUIRE(tracker.GetExpiry() == 1s);

  tracker.Update(1000 /*ms*/);
  REQUIRE(tracker.Passed());
  REQUIRE(tracker.GetExpiry() == 0s);
}