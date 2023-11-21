
#ifndef Registry_hh
#define Registry_hh

#include "utils.hpp"

class Player;

/**
 * Since the main program does not know how much players will be inherited
 * from the Player class, we use a registration and factory pattern.
 * Quite magic for beginners.
 */

class Registry {

public:
    using Factory = Player *(*)();

    static int Register(const char *name, Factory fact);

    static Player *newPlayer(std::string name);

    static void printPlayers(std::ostream &os);
};

#define stringification(s) #s
#define RegisterPlayer(x)                                                                          \
    static int registration = Registry::Register(stringification(x), x::factory)

#endif
