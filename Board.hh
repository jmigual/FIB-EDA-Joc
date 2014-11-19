#ifndef Board_hh
#define Board_hh

#include "Utils.hh"

using namespace std;


//***********************************************************
// Constants que els alumnes podeu usar dins del vostre codi,
// però per a les quals el valor concret importa molt.

const int MAX = 60;          // Dimensions del tauler.

const int NUM_SOLDATS = 20;  // Nombre inicial de soldats per equip.
const int NUM_HELIS   =  2;  // Nombre d'helicòpters per equip.
const int NUM_POSTS   = 16;  // Nombre total de posts.

const int VIDA        = 100; // Punts de vida inicials dels soldats.
const int DANY_BOSC   =  20; // Dany mínim per atac rebut al bosc    (dany màxim: el doble).
const int DANY_GESPA  =  50; // Dany mínim per atac rebut a la gespa (dany màxim: el doble).

const int TORNS_NAPALM = 30; // Torns per tornar a tenir napalm.
const int ABAST        =  2; // Hi ha destrucció en un quadrat 2*ABAST+1.

const int    CREMA_BOSC   = 10;   // Torns que crema una casella de bosc.
const int    CREMA_ALTRE  =  5;   // Torns que crema una altra casella.
const double PROB_FOC     =  0.1; // Probabilitat de propagació del foc.

const int TORNS_BAIXAR = 20; // Max nombre de torns per saltar.
const int MAX_BAIXEN   =  4; // Max nombre de paracaigudistes d'un equip
                             // que poden saltar en el mateix torn.

const int TORNS_JOC = 200;   // Torns que dura el joc.

const int VALOR_ALT  = 4000; // Valor dels posts més   valuosos.
const int VALOR_BAIX = 1000; // Valor dels posts menys valuosos.


//***********************************************************
// Constants que els alumnes hauríeu d'usar dins del vostre codi,
// perquè el seu valor concret no importa.

const int BOSC     = 1;
const int GESPA    = 2;
const int AIGUA    = 3;
const int MUNTANYA = 4;

const int SOLDAT = 1;
const int HELI   = 2;

const int AVANCA1         = 1;
const int AVANCA2         = 2;
const int CONTRA_RELLOTGE = 3;
const int RELLOTGE        = 4;
const int NAPALM          = 5;


//***********************************************************
// Tipus públic: defineix posicions dins del tauler.

struct Posicio {
  int x, y;

  // Posició no inicialitzada.
  Posicio() { }

  // Posició (x, y).
  Posicio(int x, int y) : x(x), y(y) { }

  // Posició origen.
  Posicio(int) : x(0), y(0) { }

  // Comparació de posicions.
  bool operator!=(const Posicio &p) const {
    return x != p.x or y != p.y;
  }
};

// Retorna si (x,y) és en els límits del tauler.
inline bool valid(int x, int y) {
  return 0 <= x and x < MAX and 0 <= y and y < MAX;
}

// Retorna si p és en els límits del tauler.
inline bool valid(const Posicio& p) {
  return valid(p.x, p.y);
}


//***********************************************************
// Tipus públic: defineix els posts.

struct Post {
  int     equip;  // Equip que té el post, -1 si encara ningú.
  Posicio   pos;  // Posició del post.
  int     valor;  // Valor del post.

  // Post no inicialitzat.
  Post() { }
  // Post de l'equip donat, situat a (x, y), amb valor v.
  Post(int equip, int x, int y, int v) : equip(equip), pos(Posicio(x, y)), valor(v) { }

  // Aquests tres mètodes probablement no seran útils als estudiants.
  Post(int) : equip(0), pos(0), valor(0) { }
  Post(const Post &p) : equip(p.equip), pos(p.pos), valor(p.valor) { }
  bool operator!=(const Post &p) const {
    return equip != p.equip or pos != p.pos or valor != p.valor;
  }
};


//***********************************************************
// Tipus públic: informació de cada soldat o helicòpter.

struct Info {
  int id;              // Identificador.
  int tipus;           // Soldat o helicòpter.
  int equip;           // 1, 2, 3, o 4
  Posicio pos;         // Posicio (central, si helicòpter) on està.
  int vida;            // Punts de vida (-1 si helicòpter).
  int orientacio;      // Cap a on mira l'helicòpter,
                       // 0 = Sud, 1 = Est, 2 = Nord, 3 = Oest
                       // (-1 si soldat).
  int napalm;          // Torns fins al següent napalm disponible (-1 si soldat).
  vector<int> paraca;  // Vector de comptadors de temps dels paracaigudistes, ordenats de petit a gran.
                       // Com més petit és el comptador, menys torns té el paracaigudista per saltar.
                       // (buit si soldat).

  // Constructor per a Info amb tots els camps definits.
  Info(int id, int tipus, int equip, int x, int y, int vida, int orientacio,
       int napalm, const vector<int>& paraca) : id(id), tipus(tipus), equip(equip), pos(x, y),
						vida(vida), orientacio(orientacio), napalm(napalm),
						paraca(paraca) { }

  // Aquests dos mètodes probablement no seran útils als estudiants.
  Info() : id(0), tipus(0), equip(0), pos(0), vida(0), orientacio(0),
           napalm(0), paraca() { }

  bool operator!=(const Info &i) const {
    return id != i.id or tipus != i.tipus or equip != i.equip or pos != i.pos
      or vida != i.vida or orientacio != i.orientacio
      or napalm != i.napalm or paraca != i.paraca;
  }
};

// Info que es retorna per marcar que algunes comandes han estat errònies.
const Info NUL(-1, -1, -1, -1, -1, -1, -1, -1, vector<int>());


typedef vector<atzar>    	  VA;
typedef vector<int>      	  VE;
typedef vector<VE>       	 VVE;
typedef vector<VVE>      	VVVE;
typedef vector<Posicio>  	  VP;
typedef vector<VP>       	 VVP;
typedef vector<Post>     	 VP2;
typedef map<int, Info>   	 MEI;
typedef MEI::iterator    	  MI;
typedef MEI::const_iterator      MIc;
typedef set<int>                  SE;
typedef map<int, Posicio>        MEP;
typedef pair<int, Posicio>        P2;
typedef map<int, int>            MEE;
typedef pair<int, int>             P;


//***********************************************************
// classe Board. Els estudiants només poden fer servir la part
// pública d'aquesta classe (la part privada de la classe es fa
// servir per a la simulació del joc).

class Action;

class Board {

public:

  // Retorna el vostre equip (1-4).
  int qui_soc() const;

  // Retorna el nom de l'equip e.
  // Retorna l'string buit si l'equip és incorrecte.
  string nom(int e) const;

  // Retorna el torn actual.
  // Val 0 si és el primer torn. Val 199 si és l'últim torn.
  int quin_torn() const;

  // Retorna el número de torns restants (inclòs aquest).
  // Val 200 si és el primer torn. Val 1 si és l'últim torn.
  int torns_restants() const;

  // Retorna quin terreny hi ha a (x, y).
  // Pot ser BOSC, GESPA, AIGUA o MUNTANYA.
  // Aquestes constants estan predefinides.
  // Retorna -1 si la posició és incorrecta.
  int que(int x, int y) const;

  // Retorna el temps que estarà encès (com a mínim) el foc de (x, y).
  // Retorna 0 si a (x, y) no hi ha foc.
  // Retorna -1 si la posició és incorrecta.
  int temps_foc(int x, int y) const;

  // Retorna l'identificador del soldat que es troba a (x, y).
  // Retorna 0 si no n'hi ha cap.
  // Retorna -1 si la posició és incorrecta.
  int quin_soldat(int x, int y) const;

  // Retorna l'identificador de l'helicòpter que es troba centrat a (x, y).
  // Retorna 0 si no n'hi ha cap.
  // Retorna -1 si la posició és incorrecta.
  int quin_heli(int x, int y) const;

  // Retorna de quin equip és el post situat a (x, y).
  // Retorna 0 si a (x, y) no hi ha cap post.
  // Retorna -1 si el post encara no és de cap equip,
  // o si la posició és incorrecta.
  int de_qui_post(int x, int y) const;

  // Retorna el valor del post de (x, y) (VALOR_BAIX o VALOR_ALT).
  // Retorna 0 si no hi ha post.
  // Retorna -1 si la posició és incorrecta.
  int valor_post(int x, int y) const;

  // Retorna els identificadors dels soldats de l'equip e.
  // Retorna un vector buit si l'equip és incorrecte.
  vector<int> soldats(int e) const;

  // Retorna els identificadors dels helicòpters de l'equip e.
  // Retorna un vector buit si l'equip és incorrecte.
  vector<int> helis(int e) const;

  // Retorna informació de tots els posts
  // (de qui són en aquest moment, on es troben, i quin és el seu valor).
  vector<Post> posts() const;

  // Retorna la informació del soldat o helicòpter amb identificador id.
  // Els camps que inclou són:
  //    * id          (identificador),
  //    * tipus       (SOLDAT o HELI, són dues constants predefinides),
  //    * equip       (entre 1 i 4),
  //    * pos         (posició del soldat o posició central de l'helicòpter, segons el tipus),
  //    * vida        (punts de vida del soldat, -1 per als helicòpters),
  //    * orientacio  (cap a on mira l'helicòpter: 0 -> S, 1 -> E, 2 -> N, 3 -> W, -1 per als soldats),
  //    * napalm      (torns que falten fins a tenir napalm disponible, -1 per als soldats).
  //
  // El mètode dades(id) retorna NUL (una constant predefinida de tipus Info, amb tots els camps a -1)
  // si no hi ha cap efectiu amb aquest identificador.
  Info dades(int id) const;

  // Retorna la puntuació de l'equip e.
  // Retorna -1 si l'equip és incorrecte.
  int puntuacio(int e) const;

  // Retorna el temps de CPU consumit fins al moment (sobre el total disponible) de l'equip e.
  // És un valor de (0%) a 1 (100%), o -1 si s'ha exhaurit.
  // Retorna -1 si l'equip és incorrecte.
  double status(int e) const;


  //**********************************************
  // Funcions d'atzar.

  // Retorna un nombre aleatori en [e, d].
  int uniforme(int e, int d);

  // Retorna una permutació aleatòria de 0, 1, .., n - 1.
  vector<int> permutacio(int n);

  // Retorna true amb probabilitat p.
  bool probabilitat(double p);



  //***************************************************************************
  //   EL QUE HI HA EN AQUEST FITXER A PARTIR D'AQUÍ NO US SERVEIX PER A RES.
  //***************************************************************************

  // Constructor buit.
  Board();


private:

  static const char code[5];

  // Allow access to the private part of Board.
  friend class Game;
  friend class SecGame;
  friend class Player;

  // Això és per facilitar la programació dels codis del tauler.
  VE Xcreu, Ycreu; // veins en creu
  VE Xdiag, Ydiag; // veins en diagonal
  VE Xvei,  Yvei;  // tots els veins, el centre inclòs

  int    torn;
  int jugador; // 1-4: jugador concret
  atzar     A; // generador d'atzar per al tauler
  VVE terreny; // que hi ha a (x, y): bosc, gespa, llac o muntanya
  VVE     foc; // torns que durara el foc de (x, y): 0-> no n'hi ha
  VVVE   iden; // qui hi ha a (nivell, x, y): 0-> ningu
  VVE     cel; // caselles actualment ocupades del cel (0 lliure, 1 ocupada)
  VVE   quipost; // post  de (x, y): 0-> cap, -1-> lliure, 1-4-> equip
  VVE valorpost; // valor de (x, y): 0-> no hi ha post, altrament
                 // VALOR_BAIX o VALOR_ALT
  VP2    post; // informacio de tots els posts

  MEI    dada; // dades de cada id
  int     nou; // nou identificador mai usat abans

  VA       at; // per a cada equip, generador d'atzar independent
  VVE  soldat; // per a cada equip, vector dels identificadors dels soldats
  VVE    heli; // per a cada equip, vector dels identificadors dels helis

  vector<string>  names_;
  vector<double> status_; // cpu status. <-1: dead, 0..1: %of cpu time limit

  double foo;


  double& statusPriv(int e);

  bool es_jugador();

  int  nou_id();
  int  nou_soldat(int e, int x, int y);
  void marca_heli   (int x, int y);
  void desmarca_heli(int x, int y);
  int  nou_heli(int e, int x, int y, int orientacio);
  void mata_soldat(int id);
  int  diferent(int e);
  void ini_veins_en_creu();
  void propaga_foc();
  void fes_torn_soldat(int id, int x2, int y2, vector<Action>& done);
  void fes_torn_soldats(vector<P2>& V, vector<Action>& done);
  void fes_torn_heli(int id, int ins, vector<Action>& done);
  void fes_torn_helis(vector<P>& ordres2, vector<Action>& done);
  void paracaigudistes_baixen(const VVP& ordres3, vector<Action>& done);

  // Construeix un tauler llegint el primer torn des d'un stream.
  Board (istream& is);

  // Escriu el tauler a un stream.
  void print (ostream& os) const;

  // Escriu el header a un stream.
  void print_header(ostream& os) const;

  // Calcula el següent tauler aplicant les accions donades al tauler
  // actual. També retorna les accions finalment realitzades.
  Board next (const vector<Action>& asked, vector<Action>& done) const;

  // Comprova invariants del tauler. Per debugging.
  bool ok(void) const;
};


extern int seed;

inline int Board::qui_soc() const {
  return jugador;
}

inline string Board::nom(int e) const {
  if (e < 1 or e > 4) {
    error("nom invocat amb " + i2s(e));
    return "";
  }
  return names_.at(e-1);
}

inline int Board::quin_torn() const {
  return torn;
}

inline int Board::torns_restants() const {
  return TORNS_JOC - torn;
}

inline int Board::que(int x, int y) const {
  if (x < 0 or x >= MAX or y < 0 or y >= MAX) {
    error("que invocat amb " + i2s(x) + " " + i2s(y));
    return -1;
  }
  return terreny[x][y];
}

inline int Board::temps_foc(int x, int y) const {
  if (x < 0 or x >= MAX or y < 0 or y >= MAX) {
    error("temps_foc invocat amb " + i2s(x) + " " + i2s(y));
    return -1;
  }
  return foc[x][y];
}

inline int Board::quin_soldat(int x, int y) const {
  if (x < 0 or x >= MAX or y < 0 or y >= MAX) {
    error("quin_soldat invocat amb " + i2s(x) + " " + i2s(y));
    return -1;
  }
  return iden[SOLDAT][x][y];
}

inline int Board::quin_heli(int x, int y) const {
  if (x < 0 or x >= MAX or y < 0 or y >= MAX) {
    error("quin_heli invocat amb " + i2s(x) + " " + i2s(y));
    return -1;
  }
  return iden[HELI][x][y];
}

inline int Board::de_qui_post(int x, int y) const {
  if (x < 0 or x >= MAX or y < 0 or y >= MAX) {
    error("de_qui_post invocat amb " + i2s(x) + " " + i2s(y));
    return -1;
  }
  return quipost[x][y];
}

inline int Board::valor_post(int x, int y) const {
  if (x < 0 or x >= MAX or y < 0 or y >= MAX) {
    error("valor_post invocat amb " + i2s(x) + " " + i2s(y));
    return -1;
  }
  return valorpost[x][y];
}

inline vector<int> Board::soldats(int e) const {
  if (e < 1 or e > 4) {
    error("soldats invocat amb " + i2s(e));
    return VE(0);
  }
  return soldat[e];
}

inline vector<int> Board::helis(int e) const {
  if (e < 1 or e > 4) {
    error("helis invocat amb " + i2s(e));
    return VE(0);
  }
  return heli[e];
}

inline vector<Post> Board::posts() const {
  return post;
}

inline Info Board::dades(int id) const {
  MIc p = dada.find(id);
  if (p == dada.end()) {
    error("dades invocat amb " + i2s(id));
    return NUL;
  }
  return p->second;
}

inline int Board::puntuacio(int e) const {
  if (e < 1 or e > 4) {
    error("puntuacio invocat amb " + i2s(e));
    return -1;
  }
  int p = 0;
  for (int i = 0; i < (int)post.size(); ++i)
    if (post[i].equip == e) p += post[i].valor;
  p += soldat[e].size();
  return p;
}

inline double Board::status(int e) const {
  if (e < 1 or e > 4) {
    error("status invocat amb " + i2s(e));
    return -1;
  }
  return status_[e-1];
}

inline double& Board::statusPriv(int e) {
  assert(1 <= e and e <= 4);
  return status_[e-1];
}

inline int Board::uniforme(int e, int d) {
  assert(es_jugador());
  if (e > d) {
    error("uniforme invocat amb " + i2s(e) + " " + i2s(d));
    return -1;
  }
  return at[jugador].uniforme(e, d);
}

inline vector<int> Board::permutacio(int n) {
  assert(es_jugador());
  if (n < 0) {
    error("permutacio invocat amb " + i2s(n));
    return VE(0);
  }
  return at[jugador].permutacio(n);
}

inline bool Board::probabilitat(double p) {
  assert(es_jugador());
  if (p < 0 or p > 1) {
    error("probabilitat invocat amb " + d2s(p));
    return false;
  }
  return at[jugador].probabilitat(p);
}

inline Board::Board() { }

inline bool Board::es_jugador() { return jugador >= 1 and jugador <= 4; }

inline int Board::nou_id() {
  return nou++;
}

inline int Board::nou_soldat(int e, int x, int y) {
  int id = nou_id();
  iden[SOLDAT][x][y] = id;
  dada[id] = Info(id, SOLDAT, e, x, y, VIDA, -1, -1, VE());
  return id;
}

inline void Board::marca_heli(int x, int y) {
  for (int i = -2; i <= 2; ++i)
    for (int j = -2; j <= 2; ++j) cel[x+i][y+j] = true;
}

inline void Board::desmarca_heli(int x, int y) {
  for (int i = -2; i <= 2; ++i)
    for (int j = -2; j <= 2; ++j) cel[x+i][y+j] = false;
}

inline int Board::nou_heli(int e, int x, int y, int orientacio) {
  int id = nou_id();
  iden[HELI][x][y] = id;
  dada[id] = Info(id, HELI, e, x, y, -1, orientacio, TORNS_NAPALM, VE());
  marca_heli(x, y);
  return id;
}

inline void Board::mata_soldat(int id) {
  MI p = dada.find(id);
  int equip = p->second.equip;
  int x = p->second.pos.x;
  int y = p->second.pos.y;
  iden[SOLDAT][x][y] = 0;
  dada.erase(p);

  VE B;
  for (int k = 0; k < (int)soldat[equip].size(); ++k) {
    int a = soldat[equip][k];
    if (a != id) B.push_back(a);
  }
  swap(soldat[equip], B);
}

inline int Board::diferent(int e) {
  int x;
  do x = A.uniforme(1, 4);
  while (x == e);
  return x;
}


// Decrementa els comptadors de foc, torna els boscos cremats en gespa,
// propaga el foc, i mira si enxampa algun soldat.
inline void Board::propaga_foc() {

  VVE B(MAX, VE(MAX, 0));
  for (int x = 0; x < MAX; ++x)
    for (int y = 0; y < MAX; ++y)
      if (foc[x][y]) {
	--foc[x][y];
	if (terreny[x][y] == BOSC) {
	  if (foc[x][y] == 0) terreny[x][y] = GESPA;
	  for (int k = 0; k < 4; ++k) {
	    int xx = x + Xcreu[k];
	    int yy = y + Ycreu[k];
	    if (terreny[xx][yy] == BOSC and foc[xx][yy] == 0)
	      B[xx][yy] = true;
	  }
	}
      }

  for (int x = 0; x < MAX; ++x)
    for (int y = 0; y < MAX; ++y)
      if (B[x][y] and A.probabilitat(PROB_FOC)) {
	foc[x][y] = CREMA_BOSC;
	int id = iden[SOLDAT][x][y];
	if (id) {
	  int nou_equip = diferent(dada[id].equip);
	  int heli_id = heli[nou_equip][A.uniforme(0, int(heli[nou_equip].size())-1)];
	  dada[heli_id].paraca.push_back(TORNS_BAIXAR);
	  mata_soldat(id);
	}
      }
}


#endif
