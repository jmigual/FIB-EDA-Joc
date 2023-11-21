#include "Game.hh"
#include "Registry.hh"

using namespace std;

void Game::run(const vector<string>& names, istream& is, ostream& os) {

  cerr << "info: loading game" << endl;
  Board b0(is);
  cerr << "info: loaded game" << endl;

  if (int(names.size()) != 4) {
    cerr << "error: wrong number of players." << endl;
    exit(EXIT_FAILURE);
  }

  vector<Player*> players;        
  for (int player = 0; player < 4; ++player) {
    string name = names[player];
    cerr << "info: loading player " << name << endl;
    players.push_back(Registry::new_player(name));
    b0.names_[player] = name;
  }
  cerr << "info: players loaded" << endl;

  os << "Game" << endl << endl;
  b0.print_header(os);
  b0.print(os);

  for (int round = 1; round <= TORNS_JOC; ++round) {
    cerr << "info: start round " << round << endl;
    os << "actions_asked" << endl;
    vector<Action> asked(1);
    for (int player = 0; player < 4; ++player) {
      cerr << "info:     start player " << player + 1 << endl;
      players[player]->reset(b0, Action());
      players[player]->jugador = player+1;
      players[player]->play();
      asked.push_back(*players[player]);

      os << endl << player + 1 << endl;
      Action(*players[player]).print(os);
      cerr << "info:     end player " << player + 1 << endl;
    }
    vector<Action> done(5);
    cerr << "start next" << endl;
    Board b1 = b0.next(asked, done);
    cerr << "end next" << endl;

    os << endl << "actions_done" << endl;
    for (int player = 0; player < 4; ++player) {
      os << player + 1 << " " << endl;
      done[player+1].print(os);
    }
    os << endl;

    b1.print(os);
    b0 = b1;
    cerr << "info: end round " << round << endl;
  }

  vector<int> max_players;
  int max_score = numeric_limits<int>::min();

  for (int player = 0; player < 4; ++player) {
    cerr << "info: player " << b0.nom(player+1) << " got score " << b0.puntuacio(player+1) << endl;
    if (max_score < b0.puntuacio(player+1)) {
      max_score = b0.puntuacio(player+1);
      max_players = vector<int>(1, player);
    }
    else if (max_score == b0.puntuacio(player+1))
      max_players.push_back(player);
  }
  cerr << "info: player(s)";
  for (int k = 0; k < int(max_players.size()); ++k)
    cerr << " " << b0.nom(max_players[k] + 1);
  cerr << " got top score" << endl;

  cerr << "info: game played" << endl;
}
