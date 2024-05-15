#include "comp_club_system.h"
#include "utils.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <string>

using namespace CompClub;
using namespace CompClubUtils;

TEST_CASE("Correct time points parsed", "[time point]") {
  std::string midnight = "00:00";
  std::string minuteBeforeMidnight = "23:59";
  std::string noon = "12:01";
  std::string someTime = "13:42";
  std::string morning = "07:21";

  CHECK(std::get<TimePoint>(CompClubUtils::parseTimePoint(midnight)) == CompClub::TimePoint(std::chrono::minutes(0)));
  CHECK(
      std::get<TimePoint>(CompClubUtils::parseTimePoint(minuteBeforeMidnight)) ==
      CompClub::TimePoint(std::chrono::minutes(1439))
  );
  CHECK(std::get<TimePoint>(CompClubUtils::parseTimePoint(noon)) == CompClub::TimePoint(std::chrono::minutes(721)));
  CHECK(std::get<TimePoint>(CompClubUtils::parseTimePoint(someTime)) == CompClub::TimePoint(std::chrono::minutes(822)));
  CHECK(std::get<TimePoint>(CompClubUtils::parseTimePoint(morning)) == CompClub::TimePoint(std::chrono::minutes(441)));
}

TEST_CASE("Incorrect time points not parsed", "[time point]") {
  std::string midnight = "0:00";
  std::string minuteAfterMidnight = "23:61";
  std::string noon = "-12:00";
  std::string someTime = "25:42";
  std::string morning = "07:1";

  CHECK_THROWS(std::get<TimePoint>(CompClubUtils::parseTimePoint(midnight)));
  CHECK_THROWS(std::get<TimePoint>(CompClubUtils::parseTimePoint(minuteAfterMidnight)));
  CHECK_THROWS(std::get<TimePoint>(CompClubUtils::parseTimePoint(noon)));
  CHECK_THROWS(std::get<TimePoint>(CompClubUtils::parseTimePoint(someTime)));
  CHECK_THROWS(std::get<TimePoint>(CompClubUtils::parseTimePoint(morning)));
}
