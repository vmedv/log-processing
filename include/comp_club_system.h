#pragma once

#include <chrono>
#include <cstddef>
#include <deque>
#include <functional>
#include <istream>
#include <set>
#include <string>
#include <vector>

namespace CompClub {

using TimePoint = std::chrono::time_point<std::chrono::local_t, std::chrono::minutes>;

class Event {
private:
  std::size_t _id;
  TimePoint _time;

public:
  virtual ~Event() = default;
  Event(std::size_t id, TimePoint time);
  virtual std::size_t getId() const;
  virtual TimePoint getTime() const;
};

class ClientEvent : public Event {
private:
  std::string _name;

public:
  ~ClientEvent() override = default;
  ClientEvent(std::size_t id, TimePoint time, std::string name);
  std::string getName() const;
};

class ClientPlacedEvent : public ClientEvent {
private:
  std::size_t _desk;

public:
  ~ClientPlacedEvent() override = default;
  ClientPlacedEvent(std::size_t id, TimePoint time, std::string name, std::size_t desk);
  std::size_t getDesk() const;
};

class ErrorEvent : public Event {
private:
  std::string _error;

public:
  ~ErrorEvent() override = default;
  ErrorEvent(std::size_t id, TimePoint time, std::string error);
  std::string getError() const;
};

struct EventParseError {
  std::string str;
};

using EventExpected = std::variant<std::shared_ptr<Event>, EventParseError>;
EventExpected parseEvent(std::istream& is);

std::string printEvent(const std::shared_ptr<Event>& e);

class CompClubConfig {
private:
  std::vector<std::pair<std::string, TimePoint>> _desks;
  std::vector<std::pair<std::size_t, std::chrono::minutes>> _summary;
  TimePoint _opened;
  TimePoint _closed;
  unsigned long long _price;
  std::deque<std::string> _waiting;

  static const std::function<bool(const std::shared_ptr<ClientEvent>& a, const std::shared_ptr<ClientEvent>& b)>
      _client_name_cmp;
  using _name_cmp_t = decltype(_client_name_cmp);

public:
  CompClubConfig(std::size_t desks, TimePoint opened, TimePoint closed, unsigned long long price);

  TimePoint getOpened() const;
  TimePoint getClosed() const;

  std::set<std::shared_ptr<ClientEvent>, _name_cmp_t> close();

  const std::vector<std::pair<std::size_t, std::chrono::minutes>>& moneySummary() const;

  std::optional<std::shared_ptr<Event>> processEvent(std::shared_ptr<Event> e);
};

} // namespace CompClub
