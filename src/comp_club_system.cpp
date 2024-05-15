#include "comp_club_system.h"

#include "utils.h"

#include <bits/chrono.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>

namespace CompClub {
Event::Event(std::size_t id, TimePoint time)
    : _id(id)
    , _time(time) {}

std::size_t Event::getId() const {
  return _id;
}

TimePoint Event::getTime() const {
  return _time;
}

ClientEvent::ClientEvent(std::size_t id, TimePoint time, std::string name)
    : Event(id, time)
    , _name(std::move(name)) {}

std::string ClientEvent::getName() const {
  return _name;
}

ClientPlacedEvent::ClientPlacedEvent(std::size_t id, TimePoint time, std::string name, std::size_t desk)
    : ClientEvent(id, time, std::move(name))
    , _desk(desk) {}

std::size_t ClientPlacedEvent::getDesk() const {
  return _desk;
}

ErrorEvent::ErrorEvent(std::size_t id, TimePoint time, std::string error)
    : Event(id, time)
    , _error(std::move(error)) {}

std::string ErrorEvent::getError() const {
  return _error;
}

EventExpected parseEvent(std::istream& input) {
  std::string acc;
  std::string time_str;
  std::getline(input, time_str, ' ');
  acc += time_str + ' ';

  auto parsed_time = CompClubUtils::parseTimePoint(time_str);
  if (!std::get_if<TimePoint>(&parsed_time)) {
    std::string tmp;
    std::getline(input, tmp);
    return EventExpected{EventParseError{acc + tmp}};
  }
  TimePoint time = std::get<TimePoint>(parsed_time);

  auto parsed_id = CompClubUtils::parseNumber(input, ' ');
  if (!std::get_if<std::size_t>(&parsed_id)) {
    std::string tmp;
    std::getline(input, tmp, '\n');
    return EventExpected{EventParseError{acc + std::get<CompClubUtils::NumParseError>(parsed_id).str + " " + tmp}};
  }
  std::size_t id = std::get<std::size_t>(parsed_id);
  acc += std::to_string(id) + ' ';

  std::string name;
  std::getline(input, name);
  auto ws = std::find(name.begin(), name.end(), ' ');
  std::string rest;
  if (ws != name.end()) {
    rest = name.substr(ws - name.begin() + 1);
    name = name.substr(0, ws - name.begin());
    acc += name + ' ';
  } else {
    acc += name;
  }
  if (!CompClubUtils::validateName(name)) {
    return EventExpected{EventParseError{acc + rest}};
  }

  std::stringstream rest_str(rest);
  if (rest.empty() && (id == 1 || id == 3 || id == 4)) {
    return EventExpected{std::shared_ptr<Event>(new ClientEvent(id, time, name))};
  }

  if (id != 2) {
    return EventExpected{EventParseError{acc + rest}};
  }
  auto parsed_desk = CompClubUtils::parseNumber(rest_str, '\n');
  if (!std::get_if<std::size_t>(&parsed_desk)) {
    std::string tmp;
    std::getline(rest_str, tmp);
    return EventExpected{EventParseError{acc + std::get<CompClubUtils::NumParseError>(parsed_desk).str + " " + tmp}};
  }
  std::size_t desk = std::get<std::size_t>(parsed_desk);
  acc += std::to_string(desk) + ' ';

  std::string tmp;
  std::getline(rest_str, tmp);
  if (!tmp.empty()) {
    return EventExpected{EventParseError{acc + tmp}};
  }

  return EventExpected{std::shared_ptr<Event>(new ClientPlacedEvent(id, time, name, desk))};
}

std::string printEvent(const std::shared_ptr<Event>& e) {
  std::stringstream acc;
  acc << std::format("{:%H:%M}", e->getTime()) << ' ' << std::to_string(e->getId()) << ' ';
  if (auto ce = dynamic_cast<ClientEvent*>(e.get())) {
    acc << ce->getName();
    if (auto cpe = dynamic_cast<ClientPlacedEvent*>(ce)) {
      acc << ' ' << cpe->getDesk();
    }
  } else if (auto ee = dynamic_cast<ErrorEvent*>(e.get())) {
    acc << ee->getError();
  }
  return acc.str();
}

CompClubConfig::CompClubConfig(std::size_t desks, TimePoint opened, TimePoint closed, unsigned long long price)
    : _desks(desks)
    , _opened(opened)
    , _closed(closed)
    , _price(price)
    , _summary(desks) {}

TimePoint CompClubConfig::getOpened() const {
  return _opened;
}

TimePoint CompClubConfig::getClosed() const {
  return _closed;
}

std::optional<std::shared_ptr<Event>> CompClubConfig::processEvent(std::shared_ptr<Event> e) {
  auto is_desk_present = [e = e.get()](const std::pair<std::string, std::optional<TimePoint>>& client) {
    return client.first == dynamic_cast<ClientEvent*>(e)->getName();
  };
  switch (e->getId()) {
  case 1: {
    if (e->getTime() < this->getOpened() || e->getTime() > this->getClosed()) {
      return std::optional<std::shared_ptr<Event>>{new ErrorEvent{13, e->getTime(), std::string{"NotOpenedYet"}}};
    }
    if (std::any_of(_desks.begin(), _desks.end(), is_desk_present) ||
        std::any_of(_waiting.begin(), _waiting.end(), [e](const std::string& client) {
          return dynamic_cast<ClientEvent*>(e.get())->getName() == client;
        })) {
      return std::optional<std::shared_ptr<Event>>{new ErrorEvent{13, e->getTime(), std::string{"YouShallNotPass"}}};
    }
    _waiting.push_back(dynamic_cast<ClientEvent*>(e.get())->getName());
    return std::optional<std::shared_ptr<Event>>{};
  }
  case 2: {
    if (std::none_of(_desks.begin(), _desks.end(), is_desk_present) &&
        std::none_of(_waiting.begin(), _waiting.end(), [e](const std::string& client) {
          return dynamic_cast<ClientEvent*>(e.get())->getName() == client;
        })) {
      return std::optional<std::shared_ptr<Event>>{new ErrorEvent{13, e->getTime(), std::string{"ClientUnknown"}}};
    }
    if (!_desks[dynamic_cast<ClientPlacedEvent*>(e.get())->getDesk() - 1].first.empty()) {
      return std::optional<std::shared_ptr<Event>>{new ErrorEvent{13, e->getTime(), std::string{"PlaceIsBusy"}}};
    }
    auto name = dynamic_cast<ClientEvent*>(e.get())->getName();
    _desks[dynamic_cast<ClientPlacedEvent*>(e.get())->getDesk() - 1].first = name;
    _desks[dynamic_cast<ClientPlacedEvent*>(e.get())->getDesk() - 1].second = e->getTime();
    _waiting.erase(std::find(_waiting.begin(), _waiting.end(), name));
    return std::optional<std::shared_ptr<Event>>{};
  }
  case 3:
    if (std::any_of(_desks.begin(), _desks.end(), [](const std::pair<std::string, std::optional<TimePoint>>& place) {
          return place.first.empty();
        })) {
      return std::optional<std::shared_ptr<Event>>{new ErrorEvent{13, e->getTime(), std::string{"ICanWaitNoLonger!"}}};
    }
    if (_waiting.size() > _desks.size()) {
      return std::optional<std::shared_ptr<Event>>{
          new ClientEvent{11, e->getTime(), dynamic_cast<ClientEvent*>(e.get())->getName()}
      };
    }
    return std::optional<std::shared_ptr<Event>>{};
  case 4:
    if (std::none_of(_desks.begin(), _desks.end(), is_desk_present)) {
      return std::optional<std::shared_ptr<Event>>{new ErrorEvent{13, e->getTime(), std::string{"ClientUnknown"}}};
    }
    auto it =
        std::find_if(_desks.begin(), _desks.end(), [e](const std::pair<std::string, std::optional<TimePoint>>& client) {
          return client.first == dynamic_cast<ClientEvent*>(e.get())->getName();
        });
    it->first = "";
    _summary[it - _desks.begin()].first +=
        std::chrono::ceil<std::chrono::hours>(e->getTime() - it->second).count() * _price;
    _summary[it - _desks.begin()].second += e->getTime() - it->second;
    if (!_waiting.empty()) {
      it->first = _waiting.front();
      it->second = e->getTime();
      _waiting.pop_front();
      return std::optional<std::shared_ptr<Event>>{new ClientPlacedEvent{
          12,
          e->getTime(),
          dynamic_cast<ClientEvent*>(e.get())->getName(),
          static_cast<std::size_t>(it - _desks.begin() + 1)
      }};
    } else {
      return std::optional<std::shared_ptr<Event>>{};
    }
  }

  return std::optional<std::shared_ptr<Event>>{new ErrorEvent{42, e->getTime(), std::string{"assertion error!"}}};
}

const std::function<bool(const std::shared_ptr<ClientEvent>& a, const std::shared_ptr<ClientEvent>& b)>
    CompClubConfig::_client_name_cmp =
        [](const std::shared_ptr<ClientEvent>& a, const std::shared_ptr<ClientEvent>& b) {
          return a->getName() < b->getName();
        };

std::set<std::shared_ptr<ClientEvent>, CompClubConfig::_name_cmp_t> CompClubConfig::close() {
  std::set<std::shared_ptr<ClientEvent>, _name_cmp_t> close_events(_client_name_cmp);

  for (std::size_t desk = 0; desk < _desks.size(); ++desk) {
    if (_desks[desk].first.empty()) {
      continue;
    }
    close_events.insert(std::make_shared<ClientEvent>(11, getClosed(), _desks[desk].first));
    _summary[desk].first += std::chrono::ceil<std::chrono::hours>(getClosed() - _desks[desk].second).count() * _price;
    _summary[desk].second += getClosed() - _desks[desk].second;
  }
  return close_events;
}

const std::vector<std::pair<std::size_t, std::chrono::minutes>>& CompClubConfig::moneySummary() const {
  return _summary;
}

} // namespace CompClub
