#include "utils.h"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <istream>
#include <string_view>

namespace CompClubUtils {
using namespace std::chrono_literals;

inline constexpr auto H2M = std::chrono::duration_cast<std::chrono::minutes>(1h).count();
inline constexpr auto D2H = std::chrono::duration_cast<std::chrono::hours>(std::chrono::days(1)).count();

TimeExpected parseTimePoint(std::string_view time_str) {
  auto delim_idx = time_str.find(':');
  if (delim_idx != 2 || delim_idx + 1 == time_str.npos) {
    return TimeExpected(TimeParseError{});
  }
  auto hours_raw = time_str.substr(0, delim_idx);
  int hours = 0, minutes = 0;
  auto h_res = std::from_chars(hours_raw.data(), hours_raw.data() + hours_raw.size(), hours);
  if (h_res.ec == std::errc::invalid_argument || 0 > hours || hours >= D2H) {
    return TimeExpected(TimeParseError{});
  }
  auto minutes_raw = time_str.substr(delim_idx + 1);
  if (minutes_raw.size() != 2) {
    return TimeExpected(TimeParseError{});
  }

  auto m_res = std::from_chars(minutes_raw.data(), minutes_raw.data() + minutes_raw.size(), minutes);
  if (m_res.ec == std::errc::invalid_argument || 0 > minutes || minutes >= H2M) {
    return TimeExpected(TimeParseError{});
  }
  return TimeExpected{CompClub::TimePoint{std::chrono::minutes{hours * H2M + minutes}}};
}

NumExpected parseNumber(std::istream& input, char delim) {
  std::string cur_line;
  std::getline(input, cur_line, delim);
  if (input.fail()) {
    return NumExpected{NumParseError{cur_line}};
  }
  auto desks_stream = std::stringstream(cur_line);
  long long desks{};
  desks_stream >> desks;
  if (desks_stream.fail()) {
    return NumParseError{cur_line};
  }
  if (desks <= 0) {
    return NumParseError{cur_line};
  }
  if (!desks_stream.eof()) {
    return NumParseError{cur_line};
  }
  return NumExpected{static_cast<std::size_t>(desks)};
}

bool validateName(const std::string& name) {
  return !name.empty() && std::ranges::all_of(name.begin(), name.end(), [](char c) {
    return 'a' <= c && c <= 'z' || '0' <= c && c <= '9' || c == '_' || c == '-';
  });
}

} // namespace CompClubUtils
