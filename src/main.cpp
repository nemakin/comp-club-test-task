#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "club/club.h"
#include "club/config.h"
#include "utils/exceptions.h"

using namespace club;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>\n";
    return EXIT_FAILURE;
  }

  std::ifstream in(argv[1]);
  if (!in.is_open()) {
    std::cerr << "Failed to open file: " << std::quoted(argv[1]) << '\n';
    return EXIT_FAILURE;
  }

  try {
    ComputerClub club(Config::Parse(in));
    for (std::string line; std::getline(in, line) && !line.empty();) {
      auto next_event = events::FromString(line);
      try {
        club.ProcessIncomingEvent(std::move(next_event));
      } catch (std::invalid_argument&) {
        throw utils::BadFormat(line);
      }
    }
  } catch (utils::BadFormat& e) {
    std::cerr << "Bad line format:\n" << std::quoted(e.what()) << '\n';
    return EXIT_FAILURE;
  } catch (std::exception& e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }
}
