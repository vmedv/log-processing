#include "comp_club_system.h"
#include "utils.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <string>

using namespace CompClub;
using namespace CompClubUtils;

TEST_CASE("Correct events parsed", "[event]") {
  std::stringstream one("12:00 1 client");
  std::stringstream two("12:00 2 client 1");
  std::stringstream three("12:00 3 client");
  std::stringstream four("12:00 4 client");

  CHECK_NOTHROW(std::get<std::shared_ptr<Event>>(CompClub::parseEvent(one)));
  CHECK_NOTHROW(std::get<std::shared_ptr<Event>>(CompClub::parseEvent(two)));
  CHECK_NOTHROW(std::get<std::shared_ptr<Event>>(CompClub::parseEvent(three)));
  CHECK_NOTHROW(std::get<std::shared_ptr<Event>>(CompClub::parseEvent(four)));
}

TEST_CASE("Incorrect events not parsed", "[event]") {
  std::stringstream one("12:00 1 client 2");
  std::stringstream two("12:00 2 client");
  std::stringstream three("12:00 3 client 1");
  std::stringstream four("12:00 4 client 5");
  std::stringstream five("4 12:00 4 client 5");
  std::stringstream six("aboba 12:00 4 client 5");
  std::stringstream seven("12:00 4 5 aboba");

  CHECK_THROWS(std::get<std::shared_ptr<Event>>(CompClub::parseEvent(one)));
  CHECK_THROWS(std::get<std::shared_ptr<Event>>(CompClub::parseEvent(two)));
  CHECK_THROWS(std::get<std::shared_ptr<Event>>(CompClub::parseEvent(three)));
  CHECK_THROWS(std::get<std::shared_ptr<Event>>(CompClub::parseEvent(four)));
  CHECK_THROWS(std::get<std::shared_ptr<Event>>(CompClub::parseEvent(five)));
  CHECK_THROWS(std::get<std::shared_ptr<Event>>(CompClub::parseEvent(six)));
  CHECK_THROWS(std::get<std::shared_ptr<Event>>(CompClub::parseEvent(seven)));
}
