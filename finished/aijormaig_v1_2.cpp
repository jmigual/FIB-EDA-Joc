
#include "player.hpp"

using namespace std;


/**
 * Escriu el nom * del teu jugador i guarda
 * aquest fitxer amb el nom AI*.cc
 */
#define PLAYER_NAME jormaig_v1_2


/**
 * Podeu declarar constants aquí
 */

// Ens permet ometre missatges
//#define DDEBUG

const int NO_PASSAR = -1;   // Ens indica que no hem de passar
const int NO_VIST   = -2;   // Pel BFS, per marcar les caselles no vistes
const int POST      = -3;   // Ens indica que hi ha un post
const int ENEM      = -4;   // Ens indica que hi ha un soldat enemic

const int MAX_FOC   =  0;   // Màxim de torns que tolerem un foc per un BFS
const int DIRS      =  8;   // Direccions totals en les que es pot avançar
const int DIRS2     = 16;

// PERSONALITATS
const int ATACANT           = 1;    // Identificador dels atacants
const int EXPLORADOR        = 2;    // Identificador dels exploradors

// VARIABLES PRINCIPALS

const double PROP_1    = 0.8;  // Proporció de personlaitats inicial
const double PROP_2    = 0.3;  // Proporcio de personalits després de canvi
const int T_R_CANVI         = 40;   // Torns restants pel canvi de personalitat
const int MAX_NAPALM        = 5;   // Torns maxims fins al següent napalm
const int MIN_VIDA          = 30;   // Nombre de punts de vida a partir del
//                                     canvi de rol

// Arrays de direcció per una casella adjacent
//                   0  1  2   3   4   5   6   7
const int X[8]   = { 1, 1, 0, -1, -1, -1,  0,  1 };
const int Y[8]   = { 0, 1, 1,  1,  0, -1, -1, -1 };
const int INV[8] = { 4, 5, 6,  7,  0,  1,  2,  3 };
//                   0  1  2   3   4   5   6   7

// Arrays de direcció per dues caselles adjacents
//                     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
const int X2[16]   = { 2, 2, 2, 1, 0,-1,-2,-2,-2,-2,-2,-1, 0, 1, 2, 2};
const int Y2[16]   = { 0, 1, 2, 2, 2, 2, 2, 1, 0,-1,-2,-2,-2,-2,-2,-1};
const int INV2[16] = { 8, 9,10,11,12,13,14,15, 0, 1, 2, 3, 4, 5, 6, 7};
//                     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15


struct PLAYER_NAME : public Player {
    
    
    /**
     * Factory: retorna una nova instància d'aquesta classe.
     * No toqueu aquesta funció.
     */
    static Player *factory ()
    {
        return new PLAYER_NAME;
    }
    
    
    /**
     * Els atributs dels vostres jugadors es poden definir aquí.
     */
    // TYPEDEF necessaris per al codi
    typedef vector< P > VPa;
    typedef vector< VPa > VVPa;
    typedef map<int, int> MEE;
    typedef MEE::iterator MI;
    typedef vector< bool> VB;
    typedef vector< VB > VVB;
    
    // Struct que ens ajuda a buscar en un BFS que guarda distància i direcció
    struct find {
        // Direcció
        int dir;
        // Distància
        int dist;
        // Posició
        Posicio pos;
        
        find(int dir, int dist, const Posicio &pos) :
            dir(dir), dist(dist), pos(pos) {}
    };
    
    // Número de jugador, es guarda a la primera ronda
    int player;
    
    // Guarda informació de com podem anar als POST
    // first  -> direcció
    // second -> distància
    // Si és un post o una posició no vàlida tenim POST i NO_PASSAR
    VVPa dir;
    
    // Guarda informació dels soldats enemics més propers
    // first  -> direcció
    // second -> distància
    VVPa atac;
    
    // Guarda les personalitats dels soldats
    MEE pers;
    
    // Ens indica si hem conquerit tots els posts
    bool totsConq;
    
    // REDEFINICIONS DE LES FUNCIONS PRINCIPALS
    
    // Sobrecarrega Board::quin_heli(int x, int y) per poder utilitzar una
    // posició 'p'
    inline int quin_heli(const Posicio &p) const
    {
        return Board::quin_heli(p.x, p.y);
    }
    inline int quin_heli(int x, int y) const
    {
        return Board::quin_heli(x, y);
    }
    
    // Sobrecarrega Board::quin_soldat(int x, int y) per poder utilitzar una
    // Posició 'p'
    inline int quin_soldat(const Posicio &p) const
    {
        return Board::quin_soldat(p.x, p.y);
    }
    inline int quin_soldat(int x,int y) const
    {
        return Board::quin_soldat(x, y);
    }
    
    // Sobrecarrega Board::que(int x, int y) per poder utilitzar una posicó 'p'
    inline int que(const Posicio &p) const
    {
        return Board::que(p.x, p.y);
    }
    inline int que(int x, int y)
    {
        return Board::que(x, y);
    }
    
    // FUNCIONS DE LA CLASSE
    
    // Pre: cap
    // Post: retorna TRUE si a la posició p no hi ha cap condició que impedeixi
    // passar-hi
    bool passar(const Posicio &p)
    {
        // Obtenim el tipus d'element
        int q = que(p.x, p.y);
        
        return q != MUNTANYA && q != AIGUA &&
                temps_foc(p.x, p.y) <= MAX_FOC;
    }
    inline bool passar(int x, int y)
    {
        return PLAYER_NAME::passar(Posicio(x, y));
    }
    
    void cEvitaHelis(VVPa &M, const Posicio &p0)
    {
        queue< P2 > Q;
        VVB V(7, VB(7, false));
        
        // Afegim els objectius
        for (int j = 0; j < 7; ++j) {
            Q.push(P2(POST, Posicio(0, j)));
            Q.push(P2(POST, Posicio(6, j)));
        }
        for (int j = 1; j <= 5; ++j) {
            Q.push(P2(POST, Posicio(j, 0)));
            Q.push(P2(POST, Posicio(j, 6)));
        }
        
        while(!Q.empty()) {
            // Obtenim el primer element
            P2 p = Q.front();
            Q.pop();
            
            if (!V[p.second.x][p.second.y]) {
                V[p.second.x][p.second.y] = true;
                Posicio pn(p0.x + p.second.x, p0.y + p.second.y);
                
                // Primer mirem si es pot passar per la posició actual
                if (!passar(pn)) M[pn.x][pn.y] = P(NO_PASSAR, NO_PASSAR);
                // Si es pot passar comprovem que sigui una casella del voltant
                else if (p.first == POST) {
                    for (int i = 0; i < DIRS; ++i) {
                        Posicio pos(p.second.x + X[i], p.second.y + Y[i]);
                        if (pos.x >= 0 && pos.x < 7 && 
                            pos.y >= 0 && pos.y < 7) {
                            Q.push(P2(INV[i], pos));
                        }
                    }
                }
                // NO és una casella del voltant
                else {
                    M[pn.x][pn.y] = P(p.first, NO_PASSAR);
                    for (int i = 0; i < DIRS; ++i) {
                        Posicio pos(p.second.x + X[i], p.second.y + Y[i]);
                        Q.push(P2(INV[i], pos));
                    }
                }
            }
        }
    }
    
    void evitaHelis(VVPa &M)
    {
        queue< P2 > Q;
        // Hem de mirar tots els helicòpters del tauler per evitar-los tots
        for (int i = 1; i <= 4; ++i) if (player != i) {
            VE H = helis(i);
            for (int h : H) {
                Info I = dades(h);
                if (I.napalm < MAX_NAPALM) {
                    cEvitaHelis(M, Posicio(I.pos.x - 3, I.pos.y - 3));
                }
            }
        }
        
#ifdef DDEBUG
        if (quin_torn()%30 == 0) {
            cerr << "HELIS" << endl;
            for (int i = 0; i < MAX; ++i) {
                if (i == 0) {
                    cerr << " ";
                    for (int j = 0; j < MAX; ++j) {
                        if (j < 10) cerr << "  " << j;
                        else cerr << " " << j;
                    }
                    cerr << endl;
                }
                if (i < 10) cerr << " " << i << "|";
                else cerr << i << "|";
                for (int j = 0; j < MAX; ++j) {
                    if (M[i][j].first >= 0) cerr << M[i][j].first << "  ";
                    else if (M[i][j].first == NO_PASSAR) cerr << "N  ";
                    else cerr << ".  ";
                }
                cerr << endl;
                if (i == MAX - 1) {
                    cerr << " ";
                    for (int j = 0; j < MAX; ++j) {
                        if (j < 10) cerr << "  " << j;
                        else cerr << " " << j;
                    }
                    cerr << endl;
                }
            }
        }
#endif
    }
    
    // Pre: cap
    // Post: mou els soldats per tal d'atacar o explorar amb una proporcio
    // PROP_1 o PROP_2 en funció del torn que sigui
    void jugaSoldats(VE &soldats)
    {
        for (int a: soldats) {
            // Assignem o comprovem la personalitat
            MI per = pers.find(a);
            if (per == pers.end()) {
                if (torns_restants() <= T_R_CANVI)
                    pers[a] = (probabilitat(PROP_2) ? ATACANT : EXPLORADOR);
                else pers[a] = (probabilitat(PROP_1) ? ATACANT : EXPLORADOR);
            }
            
            // Guardem la personalitat
            int person = per->second;
            // Ens dirà si ja ha atact per no fer crides inútils
            bool atacat = false;
            
            // Si hem conquerit totes les bases canviem el rol a un més agressiu
            if (totsConq) person = ATACANT;
            
            // Primer comprovem si podem atacar a algú al voltant
            Info s = dades(a);
            // Comprovem les 8 direccions
            int solM = 0;
            for (int i = 0; i < DIRS; ++i) {
                // Calculem posició
                Posicio q(s.pos.x + X[i], s.pos.y + Y[i]);
                
                // Obtenim dades del soldat
                int sol = quin_soldat(q);
                Info dSol;
                // Si trobem un soldat al nostre voltant l'ataquem
                if (sol && (dSol = dades(sol)).equip != player) {
                    atacat = true;
                    if (solM == 0) solM = sol;
                    else if (dades(solM).vida > dSol.vida) solM = sol;
                }
            }
            if (solM) {
                Info sM = dades(solM);
                ordena_soldat(a, sM.pos.x, sM.pos.y);
            }
            
            // Reassignació del rol en funció de la vida, enviem kamikaze a
            // explorar
            if (s.vida < MIN_VIDA) pers[a] = ATACANT;
            
            // En funció de la personalitat fem una cosa o una altra
            // Personalitat atacant, intenta matar a tants soldats com pot
            if (!atacat && person == ATACANT) {
                if (atac[s.pos.x][s.pos.y].first >= 0) {
                    int x = s.pos.x;
                    int y = s.pos.y;
                    s.pos.x += X[atac[x][y].first];
                    s.pos.y += Y[atac[x][y].first];
                    
                    ordena_soldat(a, s.pos.x, s.pos.y);
                }
            }
            // Personalitat explorador, intenta conquerir tants posts com pot
            else if (!atacat) {
                // Si no ens trobem en una casella on ens haguem de quedar
                // ens movem
                if (dir[s.pos.x][s.pos.y].first >= 0) {
                    int x = s.pos.x;
                    int y = s.pos.y;
                    s.pos.x += X[dir[x][y].first];
                    s.pos.y += Y[dir[x][y].first];
                    ordena_soldat(a, s.pos.x, s.pos.y);
                }
            }
        }
    }
    
    // Pre: VE és un vector d'enters que conté els ID dels helicòpters;
    // Post: Mira si l'helicòpter té lloc per tirar els paracaigudistes
    void tiraParacaigudes(VE &helis)
    {
        for (int a : helis) {
            Info h = dades(a);
            if (h.paraca.size()) {
                // Per defecte no hem trobat cap lloc on tirar-ne un
                bool found = false;
                
                // Anem de 0 a 2 pels 3 nivells de profunditat als que pot
                // accedir l'helicòpter
                for (int i = 0; i < 2 && !found; ++i) {
                    
                    // Mirem per totes les direccions del voltant
                    for (int j = 0; j < DIRS && !found; ++j) {
                        Posicio q(h.pos.x + i*X[j], h.pos.y + i*Y[j]);
                        
                        // Mirem si es compleixen les condicions per llençar un
                        // soldat i el tirem
                        if (que(q.x, q.y) != AIGUA && quin_soldat(q) == 0) {
                            ordena_paracaigudista(q.x, q.y);
                            found = true;
                        }
                    }
                }
                for (int i = 0; i < DIRS && !found; ++i) {
                    Posicio q(h.pos.x + X2[i], h.pos.y + Y2[i]);
                    if (que(q) != AIGUA && quin_soldat(q) == 0) {
                        ordena_paracaigudista(q.x, q.y);
                        found = true;
                    }
                }
            }
        }
    }
    
    void updateBFS(int type, VVPa &M) {
        // Reiniciem la matriu del BFS
        M = VVPa(MAX, VPa(MAX, P(NO_VIST, NO_VIST)));
        
        // Cua on guardem totes les dades a buscar
        queue< find > Q;
        
        // En funció del tipus afegim uns valors o uns altres
        if (type == POST) {
            VP2 Pos = posts();
            for (Post y : Pos) if (y.equip != player) 
                Q.push(find(POST, 0, y.pos));
        }
        else if (type == ENEM) {
            for (int i = 1; i <= 4; ++i) {
                if (i != player) {
                    VE V = soldats(i);
                    for (int a : V) Q.push(find(ENEM, 0, dades(a).pos));
                }
            }
        }
        else cerr << "ERROR: Tipus mal escollit" << endl;
        
        // Cridem la funció que evita els helicòpters
        evitaHelis(M);
        
        while (!Q.empty()) {
            // Obtenim el primer
            find p = Q.front();
            Q.pop();
            
            // Es mira la primera posició de la cua ('p') i es comproven els
            // diferents casos
            if (M[p.pos.x][p.pos.y].first == NO_VIST) {
                
                // Mirem que no hi hagi cap obstacle
                if (!passar(p.pos)) 
                    M[p.pos.x][p.pos.y] = P(NO_PASSAR, NO_PASSAR);
                // Es comprova també si és un objectiu a trobar
                else if (p.dir == type) {
                    M[p.pos.x][p.pos.y] = P(type, 0);
                    
                    // Mirem per distància 1 a tot el voltant per evitar
                    // passar per allà on ja hi ha soldats nostres
                    for (int i = 0; i < DIRS; ++i) {
                        // Calculem la posicio
                        Posicio q(p.pos.x + X[i], p.pos.y + Y[i]);
                        if (type == POST) Q.push(find(INV[i], p.dist + 1, q));
                        else {
                            // Obtenim el soldat a la posició 'q' 
                            int sol = quin_soldat(q);
                            if (sol == 0 || dades(sol).equip != player) 
                                Q.push(find(INV[i], p.dist + 1, q));
                        }
                    }
                }
                // Si no és cap de les altres dues coses és una casella normal
                // de bosc o gespa
                else {
                    M[p.pos.x][p.pos.y] = P(p.dir, p.dist);
                    
                    for (int i = 0; i < DIRS; ++i) {
                        // Calculem la posició
                        Posicio q(p.pos.x + X[i], p.pos.y + Y[i]);
                        
                        Q.push(find(INV[i], p.dist + 1, q));
                    }
                }
            }
        }
        
#ifdef DDEBUG        
        if (quin_torn()%30 == 0) {
            if (type == POST) cerr << "EXPLORADORS" << endl;
            else cerr << "ATACANTS" << endl;
            for (int i = 0; i < MAX; ++i) {
                if (i == 0) {
                    cerr << " ";
                    for (int j = 0; j < MAX; ++j) {
                        if (j < 10) cerr << "  " << j;
                        else cerr << " " << j;
                    }
                    cerr << endl;
                }
                if (i < 10) cerr << " " << i << "|";
                else cerr << i << "|";
                for (int j = 0; j < MAX; ++j) {
                    if (M[i][j].first >= 0) cerr << M[i][j].first << "  ";
                    else if (M[i][j].first == NO_PASSAR) cerr << "N  ";
                    else if (M[i][j].first == type) cerr << "P  ";
                    else cerr << ".  ";
                }
                cerr << endl;
                if (i == MAX - 1) {
                    cerr << " ";
                    for (int j = 0; j < MAX; ++j) {
                        if (j < 10) cerr << "  " << j;
                        else cerr << " " << j;
                    }
                    cerr << endl;
                }
            }
        }
#endif
    }
    
    /**
     * Mètode play.
     *
     * Aquest mètode serà invocat una vegada cada torn.
     */
    virtual void play ()
    {
        // Valors per torns especifics
        if (quin_torn() == 0) {
            player = qui_soc();
            totsConq = false;
        }
        else if (torns_restants() == T_R_CANVI) {
            
            // Canviem totes les proporcions quan queden menys de 50 torns,
            // aquí se suposa que haurem aconseguit suficients soldats com per
            // anar a full
            for (pair<const int, int> &a : pers)
                a.second = (probabilitat(PROP_2) ? ATACANT : EXPLORADOR);
        }
        
        // Actualitzem les matrius per buscar camins
        updateBFS(POST, dir);
        updateBFS(ENEM, atac);
        
        VE h = helis(player);
        tiraParacaigudes(h);
        
        
        VE s = soldats(player);
        jugaSoldats(s);
    }
};


/**
 * No toqueu aquesta línia.
 */
RegisterPlayer(PLAYER_NAME);
