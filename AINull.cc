
#include "Player.hh"

using namespace std;


/**
 * Escriu el nom * del teu jugador i guarda 
 * aquest fitxer amb el nom AI*.cc
 */
#define PLAYER_NAME Null


/**
 * Podeu declarar constants aquí
 */




struct PLAYER_NAME : public Player {


  /**
   * Factory: retorna una nova instància d'aquesta classe.
   * No toqueu aquesta funció.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }
    

  /**
   * Els atributs dels vostres jugadors es poden definir aquí.
   */     


  /**
   * Mètode play.
   * 
   * Aquest mètode serà invocat una vegada cada torn.
   */     
  virtual void play () {
  }

    
};


/**
 * No toqueu aquesta línia.
 */
RegisterPlayer(PLAYER_NAME);

