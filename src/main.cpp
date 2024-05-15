#include "comp_club_system.h"
#include "utils.h"

#include <cstddef>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

using namespace std::chrono_literals;

static void exit_with_err(std::size_t string_num, std::string_view line) {
  std::format_to(std::ostreambuf_iterator<char>(std::cerr), "wrong format on line {}: \"{}\"\n", string_num, line);
  std::exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::format_to(std::ostreambuf_iterator<char>(std::cerr), "Usage: {} <input_file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  auto input = std::ifstream(argv[1]);
  if (input.fail()) {
    std::format_to(std::ostreambuf_iterator<char>(std::cerr), "Could not open input file: {}\n", argv[1]);
    return EXIT_FAILURE;
  }
  std::size_t string_num = 0;

  // scan desks
  string_num++;
  auto parsed_desks = CompClubUtils::parseNumber(input, '\n');
  if (!std::get_if<std::size_t>(&parsed_desks)) {
    exit_with_err(string_num, std::get<CompClubUtils::NumParseError>(parsed_desks).str);
  }
  std::size_t desks = std::get<std::size_t>(parsed_desks);
  input.ignore(0, '\n');
  // scan times
  string_num++;
  std::string open_line;
  std::getline(input, open_line, ' ');
  auto opened_r = CompClubUtils::parseTimePoint(open_line);
  std::string close_line;
  std::getline(input, close_line, '\n');
  if (!std::get_if<CompClub::TimePoint>(&opened_r)) {
    exit_with_err(string_num, open_line + " " + close_line);
  }
  auto opened = std::get<CompClub::TimePoint>(opened_r);
  auto closed_r = CompClubUtils::parseTimePoint(close_line);
  if (!std::get_if<CompClub::TimePoint>(&closed_r)) {
    exit_with_err(string_num, open_line + " " + close_line);
  }
  auto closed = std::get<CompClub::TimePoint>(closed_r);

  // scan price
  string_num++;
  auto parsed_price = CompClubUtils::parseNumber(input, '\n');
  if (!std::get_if<std::size_t>(&parsed_price)) {
    exit_with_err(string_num, std::get<CompClubUtils::NumParseError>(parsed_price).str);
  }
  std::size_t price = std::get<std::size_t>(parsed_price);
  input.ignore(0, '\n');

  auto comp_club = CompClub::CompClubConfig(desks, opened, closed, price);

  std::vector<std::shared_ptr<CompClub::Event>> queries;

  while (!input.eof()) {
    string_num++;
    auto e = CompClub::parseEvent(input);
    if (!std::get_if<std::shared_ptr<CompClub::Event>>(&e)) {
      exit_with_err(string_num, std::get<CompClub::EventParseError>(e).str);
    }
    auto event = std::get<std::shared_ptr<CompClub::Event>>(std::move(e));

    queries.push_back(event);
    input >> std::ws;
  }

  std::cout << std::format("{:%H:%M}", comp_club.getOpened()) << std::endl;
  for (auto& q : queries) {
    auto resp = comp_club.processEvent(q);
    std::cout << CompClub::printEvent(q) << std::endl;
    if (resp.has_value()) {
      std::cout << CompClub::printEvent(resp.value()) << std::endl;
    }
  }
  for (auto& q : comp_club.close()) {
    std::cout << CompClub::printEvent(q) << std::endl;
  }
  std::cout << std::format("{:%H:%M}", comp_club.getClosed()) << std::endl;
  for (std::size_t desk = 0; desk < comp_club.moneySummary().size(); ++desk) {
    auto& ref = comp_club.moneySummary()[desk];
    std::cout << desk + 1 << ' ' << ref.first << ' ' << std::format("{:%H:%M}", ref.second) << std::endl;
  }
}
