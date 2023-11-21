#ifndef Action_hh
#define Action_hh

#include "Utils.hh"
#include "Board.hh"

using namespace std;

//***********************************************************
// classe Action. Els estudiants només poden fer servir la part
// pública d'aquesta classe (la part privada de la classe es fa
// servir per a fer la simulació del joc).

class Action {

public:

  // Ordena al soldat id moure's a (i, j).
  void ordena_soldat(int id, int x, int y);

  // Ordena a l'helicòpter id l'acció codificada amb codi.
  void ordena_helicopter(int id, int codi);

  // Ordena fer saltar un paracaigudista a (i, j).
  void ordena_paracaigudista(int x, int y);

  // Crea una acció buida
  Action ()
  {   }

private:

  friend class Game;
  friend class SecGame;
  friend class Board;
  friend class Player;

  MEP ordres1; // ordres d'aquest torn a soldats
  MEE ordres2; // ordres d'aquest torn a helicopters
  VP  ordres3; // per a cada equip, ordres de baixada d'aquest torn

  /**
   * Constructor reading one action from a stream.
   */
  Action (istream& is);

  /**
   * Print the action to a stream.
   */
  void print (ostream& os) const;

};

inline void Action::ordena_soldat(int id, int x, int y) {
  ordres1[id] = Posicio(x, y);
}

inline void Action::ordena_helicopter(int id, int instruccio) {
  ordres2[id] = instruccio;
}

inline void Action::ordena_paracaigudista(int x, int y) {
  ordres3.push_back(Posicio(x, y));
}

#endif
