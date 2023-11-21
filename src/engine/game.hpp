#ifndef Game_hh
#define Game_hh

#include "action.hpp" // IWYU pragma: export
#include "board.hpp"  // IWYU pragma: export
#include "player.hpp" // IWYU pragma: export
#include "utils.hpp"  // IWYU pragma: export

/**
 * Game class.
 */

class Game {

public:
    static void run(const std::vector<std::string> &names, std::istream &is, std::ostream &os);
};

#endif
