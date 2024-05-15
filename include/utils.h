#pragma once

#include "comp_club_system.h"

namespace CompClubUtils {

struct TimeParseError {};

struct NumParseError {
  std::string str;
};

using TimeExpected = std::variant<CompClub::TimePoint, TimeParseError>;
using NumExpected = std::variant<std::size_t, NumParseError>;

TimeExpected parseTimePoint(std::string_view time_str);

NumExpected parseNumber(std::istream& input, char delim);

bool validateName(const std::string& name);

} // namespace CompClubUtils
