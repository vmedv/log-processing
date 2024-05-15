#include "comp_club_system.h"
#include "utils.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <string>

using namespace CompClub;
using namespace CompClubUtils;
using namespace std::chrono_literals;

TEST_CASE("Comp club", "[comp club]") {
  CompClubConfig cc{2, TimePoint(9min), TimePoint(99min), 10};

  SECTION("just looking") {
    std::shared_ptr<ClientEvent> came(new ClientEvent{1, TimePoint(9min), "client"});
    auto ret = cc.processEvent(came);
    REQUIRE(!ret.has_value());
    cc.close();
    CHECK(cc.moneySummary() == std::vector<std::pair<std::size_t, std::chrono::minutes>>{2});
  }

  SECTION("too early") {
    std::shared_ptr<ClientEvent> came1(new ClientEvent{1, TimePoint(8min), "client1"});

    auto ret1 = cc.processEvent(came1);
    REQUIRE(ret1.has_value());
  }

  SECTION("you again") {
    std::shared_ptr<ClientEvent> came1(new ClientEvent{1, TimePoint(10min), "client1"});
    std::shared_ptr<ClientEvent> came2(new ClientEvent{1, TimePoint(11min), "client1"});

    auto ret1 = cc.processEvent(came1);
    auto ret2 = cc.processEvent(came2);
    REQUIRE(!ret1.has_value());
    REQUIRE(ret2.has_value());
  }

  SECTION("who are you?") {
    std::shared_ptr<ClientEvent> e1(new ClientPlacedEvent{2, TimePoint(10min), "client1", 42});
    std::shared_ptr<ClientEvent> e2(new ClientEvent{4, TimePoint(10min), "client2"});

    auto ret1 = cc.processEvent(e1);
    auto ret2 = cc.processEvent(e2);
    REQUIRE(ret1.has_value());
    REQUIRE(ret2.has_value());
  }

  SECTION("want that place") {
    std::shared_ptr<ClientEvent> came1(new ClientEvent{1, TimePoint(10min), "client1"});
    std::shared_ptr<ClientEvent> sit1(new ClientPlacedEvent{2, TimePoint(11min), "client1", 1});
    std::shared_ptr<ClientEvent> came2(new ClientEvent{1, TimePoint(11min), "client2"});
    std::shared_ptr<ClientEvent> sit2(new ClientPlacedEvent{2, TimePoint(12min), "client2", 2});
    std::shared_ptr<ClientEvent> resit(new ClientPlacedEvent{2, TimePoint(13min), "client2", 1});

    auto c_ret1 = cc.processEvent(came1);
    REQUIRE(!c_ret1.has_value());
    auto s_ret1 = cc.processEvent(sit1);
    REQUIRE(!s_ret1.has_value());
    auto c_ret2 = cc.processEvent(came2);
    REQUIRE(!c_ret1.has_value());
    auto s_ret2 = cc.processEvent(sit2);
    REQUIRE(!s_ret2.has_value());
    auto r_ret = cc.processEvent(resit);
    REQUIRE(r_ret.has_value());
  }

  SECTION("big queue") {
    std::shared_ptr<ClientEvent> came1(new ClientEvent{1, TimePoint(10min), "client1"});
    std::shared_ptr<ClientEvent> came2(new ClientEvent{1, TimePoint(11min), "client2"});
    std::shared_ptr<ClientEvent> came3(new ClientEvent{1, TimePoint(12min), "client3"});
    auto ret1 = cc.processEvent(came1);
    REQUIRE(!ret1.has_value());
    auto ret2 = cc.processEvent(came2);
    REQUIRE(!ret2.has_value());
    auto ret3 = cc.processEvent(came3);
    REQUIRE(!ret3.has_value());

    std::shared_ptr<ClientEvent> waiting1(new ClientEvent{3, TimePoint(13min), "client1"});
    std::shared_ptr<ClientEvent> waiting2(new ClientEvent{3, TimePoint(14min), "client2"});
    auto w_ret1 = cc.processEvent(waiting1);
    REQUIRE(w_ret1.has_value());
    auto w_ret2 = cc.processEvent(waiting2);
    REQUIRE(w_ret2.has_value());
  }
}
