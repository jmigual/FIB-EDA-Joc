#ifndef Game_hh
#define Game_hh

#include "Utils.hh"
#include "Board.hh"
#include "Action.hh"
#include "Player.hh"

using namespace std;


/**
 * Game class.
 */

class Game {

public:

  static void run (const vector<string>& names, istream& is, ostream& os);

};


#endif
