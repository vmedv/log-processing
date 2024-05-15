#include "utils.h"

#include <catch2/catch_test_macros.hpp>

using namespace CompClub;
using namespace CompClubUtils;

TEST_CASE("Correct names validated", "[name]") {
  CHECK(CompClubUtils::validateName("42"));
  CHECK(CompClubUtils::validateName("_"));
  CHECK(CompClubUtils::validateName("---"));
  CHECK(CompClubUtils::validateName("amogus"));
  CHECK(CompClubUtils::validateName("0a1--"));
}

TEST_CASE("Incorrect names not validated", "[name]") {
  CHECK_FALSE(CompClubUtils::validateName("0 "));
  CHECK_FALSE(CompClubUtils::validateName("AMOGUS"));
  CHECK_FALSE(CompClubUtils::validateName(""));
  CHECK_FALSE(CompClubUtils::validateName("!"));
  CHECK_FALSE(CompClubUtils::validateName("фыв"));
}
