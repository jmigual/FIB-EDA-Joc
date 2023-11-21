#include "game.hpp"
#include "registry.hpp"



void Game::run(const std::vector<std::string>& names, std::istream& is, std::ostream& os) {

  std::cerr << "info: loading game" << std::endl;
  Board b0(is);
  std::cerr << "info: loaded game" << std::endl;

  if (int(names.size()) != 4) {
    std::cerr << "error: wrong number of players." << std::endl;
    exit(EXIT_FAILURE);
  }

  std::vector<Player*> players;        
  for (int player = 0; player < 4; ++player) {
    std::string name = names[player];
    std::cerr << "info: loading player " << name << std::endl;
    players.push_back(Registry::newPlayer(name));
    b0.names_[player] = name;
  }
  std::cerr << "info: players loaded" << std::endl;

  os << "Game" << std::endl << std::endl;
  b0.print_header(os);
  b0.print(os);

  for (int round = 1; round <= TORNS_JOC; ++round) {
    std::cerr << "info: start round " << round << std::endl;
    os << "actions_asked" << std::endl;
    std::vector<Action> asked(1);
    for (int player = 0; player < 4; ++player) {
      std::cerr << "info:     start player " << player + 1 << std::endl;
      players[player]->reset(b0, Action());
      players[player]->jugador = player+1;
      players[player]->play();
      asked.push_back(*players[player]);

      os << std::endl << player + 1 << std::endl;
      Action(*players[player]).print(os);
      std::cerr << "info:     end player " << player + 1 << std::endl;
    }
    std::vector<Action> done(5);
    std::cerr << "start next" << std::endl;
    Board b1 = b0.next(asked, done);
    std::cerr << "end next" << std::endl;

    os << std::endl << "actions_done" << std::endl;
    for (int player = 0; player < 4; ++player) {
      os << player + 1 << " " << std::endl;
      done[player+1].print(os);
    }
    os << std::endl;

    b1.print(os);
    b0 = b1;
    std::cerr << "info: end round " << round << std::endl;
  }

  std::vector<int> max_players;
  int max_score = std::numeric_limits<int>::min();

  for (int player = 0; player < 4; ++player) {
    std::cerr << "info: player " << b0.nom(player+1) << " got score " << b0.puntuacio(player+1) << std::endl;
    if (max_score < b0.puntuacio(player+1)) {
      max_score = b0.puntuacio(player+1);
      max_players = std::vector<int>(1, player);
    }
    else if (max_score == b0.puntuacio(player+1))
      max_players.push_back(player);
  }
  std::cerr << "info: player(s)";
  for (int k = 0; k < int(max_players.size()); ++k)
    std::cerr << " " << b0.nom(max_players[k] + 1);
  std::cerr << " got top score" << std::endl;

  std::cerr << "info: game played" << std::endl;
}
