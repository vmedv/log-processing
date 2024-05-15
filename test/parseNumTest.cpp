#include "comp_club_system.h"
#include "utils.h"

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <sstream>

using namespace CompClub;
using namespace CompClubUtils;

TEST_CASE("Correct numbers parsed", "[number]") {
  std::stringstream a("42\n");
  std::stringstream b("100000");
  std::stringstream c("1 ");

  CHECK(std::get<std::size_t>(CompClubUtils::parseNumber(a, '\n')) == 42);
  CHECK(std::get<std::size_t>(CompClubUtils::parseNumber(b, ' ')) == 100000);
  CHECK(std::get<std::size_t>(CompClubUtils::parseNumber(c, ' ')) == 1);
}

TEST_CASE("Incorrect numbers not parsed", "[number]") {
  std::stringstream a("0\n");
  std::stringstream b("-1");
  std::stringstream c("32.42 ");
  std::stringstream d(" 12");

  CHECK_THROWS(std::get<std::size_t>(CompClubUtils::parseNumber(a, ' ')));
  CHECK_THROWS(std::get<std::size_t>(CompClubUtils::parseNumber(b, ' ')));
  CHECK_THROWS(std::get<std::size_t>(CompClubUtils::parseNumber(c, ' ')));
  CHECK_THROWS(std::get<std::size_t>(CompClubUtils::parseNumber(d, ' ')));
}
