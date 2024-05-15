#include "comp_club_system.h"
#include "utils.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <memory>
#include <string>

using namespace CompClub;
using namespace CompClubUtils;
using namespace std::chrono_literals;

TEST_CASE("Events printed correctly", "[event]") {
  std::shared_ptr<ClientEvent> a(new ClientEvent{1, TimePoint(4min), "client"});
  std::shared_ptr<ClientPlacedEvent> b(new ClientPlacedEvent{2, TimePoint(44min), "client", 42});
  std::shared_ptr<ClientEvent> c(new ClientEvent{3, TimePoint(444min), "client"});
  std::shared_ptr<ClientEvent> d(new ClientEvent{4, TimePoint(100min), "client"});
  std::shared_ptr<ClientEvent> e(new ClientEvent{11, TimePoint(60min), "client"});
  std::shared_ptr<ClientPlacedEvent> f(new ClientPlacedEvent{12, TimePoint(0min), "client", 42});
  std::shared_ptr<ErrorEvent> g(new ErrorEvent{13, TimePoint(2min), "error"});

  CHECK(CompClub::printEvent(a) == "00:04 1 client");
  CHECK(CompClub::printEvent(b) == "00:44 2 client 42");
  CHECK(CompClub::printEvent(c) == "07:24 3 client");
  CHECK(CompClub::printEvent(d) == "01:40 4 client");
  CHECK(CompClub::printEvent(e) == "01:00 11 client");
  CHECK(CompClub::printEvent(f) == "00:00 12 client 42");
  CHECK(CompClub::printEvent(g) == "00:02 13 error");
};
