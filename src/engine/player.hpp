
#ifndef Player_hh
#define Player_hh

#include "utils.hpp"
#include "board.hpp"
#include "action.hpp"
#include "registry.hpp" // IWYU pragma: export

/***
 * Classe abstracta base per a jugadors.
 * *
 * Aquesta classe usa herència múltiple de Board
 * i Action, de manera que les seves operacions públiques es poden usar des de Player.
 *
 * Per a crear nous jugadors, heredeu des d'aquesta classe i registreu.
 * Vegeu com a exemples AINull.cc i AIDemo.cc.
 */

/**
 * @brief Abstract class for players.
 * 
 * This class uses multiple inheritance from Board and Action, so that its public operations can be
 * used from Player.
 * 
 * To create new players, inherit from this class and register it. See AINull.cc and AIDemo.cc as
 * examples.
 */
class Player : public Board, public Action {

public:

  //**********************************************
  // Mètodes per donar instruccions al vostre exèrcit.

  // Ordena al soldat amb identificador id moure's (o atacar) a la posició (x, y).
  // L'ordre s'ignora si no hi ha cap soldat amb identificador id,
  // si no pertany a l'equip que ho ordena,
  // si el soldat ja ha rebut una instrucció en aquest torn,
  // o si la posició a la què es pretén anar
  // no és veïna de la posició actual del soldat.
  void ordena_soldat(int id, int x, int y);

  // Ordena la instrucció codificada amb codi
  // a l'helicòpter amb identificador id.
  // L'ordre s'ignora si no hi ha cap helicòpter amb identificador id,
  // si no pertany a l'equip que ho ordena,
  // si l'helicòpter ja ha rebut una instrucció en aquest torn,
  // o si el codi no és
  // ni AVANCA1,
  // ni AVANCA2,
  // ni ESQUERRA,
  // ni DRETA,
  // ni NAPALM.
  // Aquestes cinc constants estan predefinides.
  void ordena_helicopter(int id, int codi);

  // Ordena que un paracaigudista salti a la posició (x, y).
  // L'ordre s'ignora si durant aquest torn 
  // ja s'han donat tantes ordres de salt
  // com paracaigudistes actualment disponibles en el corresponent helicòpter,
  // o tantes ordres de salt com el màxim de salts permès a cada torn,
  // o si (x, y) està fora del tauler.
  void ordena_paracaigudista(int x, int y);


  //***************************************************************************
  //   EL QUE HI HA EN AQUEST FITXER A PARTIR D'AQUÍ NO US SERVEIX PER A RES.
  //***************************************************************************

  /**
   * My play intelligence.
   * Will have to be overwritten, thus declared virtual.
   */
  virtual void play () {
  };

private:

  friend class Game;
  friend class SecGame;

  inline void reset (const Board& board, const Action& action) {
    *( Board*)this = board;
    *(Action*)this = action;
  }

  bool comprova_ordena_soldat        (int id, int x, int y, int equip);
  bool comprova_ordena_helicopter    (int id, int codi, int equip);
  bool comprova_ordena_paracaigudista(int equip, int x, int y);
  std::string codi_ordena_soldat          (int id, int x, int y, int equip);
  std::string codi_ordena_helicopter      (int id, int codi, int equip);
  std::string codi_ordena_paracaigudista  (int equip, int x, int y);

  bool prous_paracaigudistes(int equip, int x, int y);

};


inline void Player::ordena_soldat(int id, int x, int y) {
  if (! comprova_ordena_soldat(id, x, y, jugador))
    return error("ordena_soldat invocat amb " + i2s(id) + " " + i2s(x) + " "
                 + i2s(y) + " : " + codi_ordena_soldat(id, x, y, jugador));

  Action::ordena_soldat(id, x, y);
}

inline void Player::ordena_helicopter(int id, int instruccio) {
  if (! comprova_ordena_helicopter(id, instruccio, jugador))
    return error("ordena_helicopter invocat amb " + i2s(id) + " "
                 + i2s(instruccio) + " : "
                 + codi_ordena_helicopter(id, instruccio, jugador));

  Action::ordena_helicopter(id, instruccio);
}

inline void Player::ordena_paracaigudista(int x, int y) {
  if (! comprova_ordena_paracaigudista(jugador, x, y))
    return error("ordena_paracaigudista invocat amb " + i2s(x) + " " + i2s(y)
		 + " : " + codi_ordena_paracaigudista(jugador, x, y));

  Action::ordena_paracaigudista(x, y);
}

inline bool Player::comprova_ordena_soldat(int id, int x, int y, int equip) {
  MI p = dada.find(id);
  return ! (p == dada.end() || p->second.equip != equip
              || p->second.tipus != SOLDAT
              || ordres1.find(id) != ordres1.end()
              || abs(p->second.pos.x - x) > 1
              || abs(p->second.pos.y - y) > 1);
}

inline bool Player::comprova_ordena_helicopter(int id, int instruccio, int equip) {
  MI p = dada.find(id);
  return ! (p == dada.end() || p->second.equip != equip
              || p->second.tipus != HELI
              || ordres2.find(id) != ordres2.end()
              || (instruccio != AVANCA1 && instruccio != AVANCA2
		  && instruccio != RELLOTGE && instruccio != CONTRA_RELLOTGE
		  && instruccio != NAPALM));
}

inline bool Player::comprova_ordena_paracaigudista(int equip, int x, int y) {
  return  prous_paracaigudistes(equip, x, y) &&
    ! ((int)ordres3.size() >= MAX_BAIXEN
	 || x < 0 || x >= MAX || y < 0 || y >= MAX);
}

inline std::string Player::codi_ordena_soldat(int id, int x, int y, int equip) {
  MI p = dada.find(id);
  if (p == dada.end()) return "identificador inexistent";
  if (p->second.equip != equip) return "equip incorrecte";
  if (p->second.tipus != SOLDAT) return "tipus incorrecte";
  if (ordres1.find(id) != ordres1.end())
    return "mes d'una ordre en aquest torn";
  if (abs(p->second.pos.x - x) > 1 || abs(p->second.pos.y - y) > 1)
    return "casella massa llunyana";
  return "???";
}

inline std::string Player::codi_ordena_helicopter(int id, int instruccio, int equip) {
  MI p = dada.find(id);
  if (p == dada.end()) return "identificador inexistent";
  if (p->second.equip != equip) return "equip incorrecte";
  if (p->second.tipus != HELI) return "tipus incorrecte";
  if (ordres2.find(id) != ordres2.end())
    return "mes d'una ordre en aquest torn";
  if (instruccio != AVANCA1 && instruccio != AVANCA2
      && instruccio != RELLOTGE && instruccio != CONTRA_RELLOTGE
      && instruccio != NAPALM) return "instruccio desconeguda";
  return "???";
}

inline std::string Player::codi_ordena_paracaigudista(int equip, int x, int y) {
  if (! prous_paracaigudistes(equip, x, y))
    return "massa instruccions de salt per un helicopter";
  if ((int)ordres3.size() >= MAX_BAIXEN)
    return "massa instruccions de salt en un mateix torn";
  if (x < 0 || x >= MAX || y < 0 || y >= MAX)
    return "posicio fora del tauler";
  return "???";
}

inline bool Player::prous_paracaigudistes(int equip, int x, int y) {
  for (int j = 0; j < (int)heli[equip].size(); ++j) {
    int heli_id = heli[equip][j];
    int x2 = dada[heli_id].pos.x;
    int y2 = dada[heli_id].pos.y;

    if (abs(x2 - x) <= 2 && abs(y2 - y) <= 2) {

      int cnt = 1;
      for (int k = 0; k < int(ordres3.size()) && cnt <= int(dada[heli_id].paraca.size()); ++k) {
	int x1 = ordres3[k].x;
	int y1 = ordres3[k].y;
	if (abs(x2 - x1) <= 2 && abs(y2 - y1) <= 2)
	  ++cnt;
      }
      return cnt <= int(dada[heli_id].paraca.size());
    }
  }   
  return false;
}

#endif
