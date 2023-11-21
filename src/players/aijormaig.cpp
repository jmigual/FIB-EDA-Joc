
#include "player.hpp"

/**
 * Escriu el nom * del teu jugador i guarda
 * aquest fitxer amb el nom AI*.cc
 */
#define PLAYER_NAME jormaig

/**
 * Podeu declarar constants aquí
 */

const int NO_PASSAR = -1; // Ens indica que no hem de passar
const int NO_VIST = -2;   // Pel BFS, per marcar les caselles no vistes
const int POST = -3;      // Ens indica que hi ha un post
const int ENEM = -4;      // Ens indica que hi ha un soldat enemic
const int ENEM2 = -4;     // Per calcular un nou bfs

const int MAX_FOC = 0; // Màxim de torns que tolerem un foc per un BFS
const int DIRS = 8;    // Direccions totals en les que es pot avançar
const int DIRS2 = 16;  // Direccions totals en les que es pot avançar 2

// ROLS
const int ATACANT = 1;    // Identificador dels atacants
const int EXPLORADOR = 2; // Identificador dels exploradors

// VARIABLES CANVIS ROL
const double PROP_1 = 0.9; // Proporció de personlaitats inicial
const double PROP_2 = 0.2; // Proporcio de personalits després de canvi
const int T_R_CANVI = 45;  // Torns restants pel canvi de personalitat
const int MAX_NAPALM = 5;  // Torns maxims fins al següent napalm
const int MIN_VIDA = 30;   // Nombre de punts de vida canvi rol

// VARIABLES PROMIG ATAC
const int BOSC_M = 30;
const int GESPA_M = 70;
const int MAX_SOLDATS = 5;

// Constants de direcció
const int SUD = 0;
const int EST = 1;
const int NORD = 2;
const int OEST = 3;

// Arrays de direcció per una casella adjacent
//                   0  1  2   3   4   5   6   7
const int X[8] = {1, 1, 0, -1, -1, -1, 0, 1};
const int Y[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const int INV[8] = {4, 5, 6, 7, 0, 1, 2, 3};
//                   0  1  2   3   4   5   6   7

// Arrays de direcció per dues caselles adjacents
//                     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
const int X2[16] = {2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2, 2};
const int Y2[16] = {0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1};
const int INV2[16] = {8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7};
//                     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15

// Arrays de direcció per als helicopters
//                  0  1   2   3
const int XH[4] = {1, 0, -1, 0};
const int YH[4] = {0, 1, 0, -1};
//                  0  1   2   3

struct PLAYER_NAME : public Player {

    /**
     * Factory: retorna una nova instància d'aquesta classe.
     * No toqueu aquesta funció.
     */
    static Player *factory() { return new PLAYER_NAME; }

    /**
     * Els atributs dels vostres jugadors es poden definir aquí.
     */

    // Struct que ens ajuda a buscar en un BFS que guarda distància i direcció
    struct find {
        // Direcció
        short dir;
        // Distància
        short dist;
        // Posició
        Posicio pos;

        find(int dir, int dist, const Posicio &pos) : dir(dir), dist(dist), pos(pos) {}

        bool operator<(const find &f) const { return this->dist < f.dist; }
    };

    // Struct que ens ajuda a buscar en un dijkstra, guarda distància i direcció
    struct find2 {
        // Posició
        Posicio pos;
        // Direcció
        short dir;
        // Direcció inicial
        short dir0;
        // Distància
        short dist;
        // Avanç incicial
        bool ava;
        find2() {}

        find2(const Posicio &pos, int dir, int dir0, int dist, bool ava) :
            pos(pos), dir(dir), dir0(dir0), dist(dist), ava(ava) {}

        find2(const find2 &f) : pos(f.pos), dir(f.dir), dir0(f.dir0), dist(f.dist), ava(f.ava) {}

        bool operator<(const find2 &f) const { return this->dist > f.dist; }
    };

    // TYPEDEF necessaris per al codi
    typedef std::pair<short, short> PS;
    typedef std::vector<PS> VPa;
    typedef std::vector<VPa> VVPa;
    typedef std::map<int, short> MES;
    typedef MES::iterator MI;
    typedef std::vector<bool> VB;
    typedef std::vector<VB> VVB;

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
    VVPa atac2;

    VVB pH; // Per veure les posicions accessibles des d'un helicòpter
    VVB pA; // True si la posició té un post accessible

    MES pers; // Guarda les personalitats dels soldats

    bool totsConq; // Ens indica si hem conquerit tots els posts

    // REDEFINICIONS DE LES FUNCIONS PRINCIPALS

    // Sobrecarrega Board::de_qui_post(int x, int y) per poder utilitzar una
    // posició 'p'
    inline int de_qui_post(int x, int y) const { return Board::de_qui_post(x, y); }
    inline int de_qui_post(const Posicio &p) const { return Board::de_qui_post(p.x, p.y); }

    // Sobrecarrega Board::quin_heli(int x, int y) per poder utilitzar una
    // posició 'p'
    inline int quin_heli(const Posicio &p) const { return Board::quin_heli(p.x, p.y); }
    inline int quin_heli(int x, int y) const { return Board::quin_heli(x, y); }

    // Sobrecarrega Board::quin_soldat(int x, int y) per poder utilitzar una
    // Posició 'p'
    inline int quin_soldat(const Posicio &p) const { return Board::quin_soldat(p.x, p.y); }
    inline int quin_soldat(int x, int y) const { return Board::quin_soldat(x, y); }

    // Sobrecarrega Board::que(int x, int y) per poder utilitzar una posicó 'p'
    inline int que(const Posicio &p) const { return Board::que(p.x, p.y); }
    inline int que(int x, int y) { return Board::que(x, y); }

    // FUNCIONS DE LA CLASSE

    // Pre: cap
    // Post: retorna TRUE si a la posició p no hi ha cap condició que impedeixi
    // passar-hi
    bool passar(const Posicio &p) {
        // Obtenim el tipus d'element
        int q = que(p.x, p.y);

        return q != MUNTANYA && q != AIGUA && temps_foc(p.x, p.y) <= MAX_FOC;
    }
    inline bool passar(int x, int y) { return this->passar(Posicio(x, y)); }

    // Pre: cap
    // Post: calcula totes les posicions que en algun moment de la partida poden
    // ser accessibles per un helicòpter i també les posicions a les que pot
    // llançar un paracaigues per tal que arribi a algun post
    void precalculaH() {
        pH = VVB(MAX, VB(MAX, true));
        for (int i = 0; i < MAX; ++i) {
            pH[0][i] = pH[MAX - 1][i] = pH[i][0] = pH[i][MAX - 1] = false;
            pH[1][i] = pH[MAX - 2][i] = pH[i][1] = pH[i][MAX - 2] = false;
            pH[2][i] = pH[MAX - 3][i] = pH[i][2] = pH[i][MAX - 3] = false;
        }
        for (int i = 2; i < MAX - 2; ++i) {
            for (int j = 2; j < MAX - 2; ++j) {
                if (que(i, j) == MUNTANYA) {
                    pH[i][j] = false;
                    for (int k = 0; k < DIRS; ++k) {
                        pH[i + X[k]][j + Y[k]] = false;
                    }
                    for (int k = 0; k < DIRS2; ++k) {
                        pH[i + X2[k]][j + Y2[k]] = false;
                    }
                }
            }
        }

        std::queue<Posicio> Q;
        pA = VVB(MAX, VB(MAX, false));
        VP2 Pos = posts();
        for (Post p : Pos)
            Q.push(p.pos);

        while (!Q.empty()) {
            Posicio p = Q.front();
            Q.pop();

            if (!pA[p.x][p.y] && passar(p)) {
                pA[p.x][p.y] = true;
                for (int i = 0; i < DIRS; ++i) {
                    Q.push(Posicio(p.x + X[i], p.y + Y[i]));
                }
            }
        }

#ifndef NDEBUG
        if (quin_torn() % 30 == 0) {
            std::cerr << "PRECALC" << std::endl;
            for (int i = 0; i < MAX; ++i) {
                if (i == 0) {
                    std::cerr << " ";
                    for (int j = 0; j < MAX; ++j) {
                        if (j < 10)
                            std::cerr << "  " << j;
                        else
                            std::cerr << " " << j;
                    }
                    std::cerr << std::endl;
                }
                if (i < 10)
                    std::cerr << " " << i << "|";
                else
                    std::cerr << i << "|";
                for (int j = 0; j < MAX; ++j) {
                    if (pH[i][j])
                        std::cerr << "#  ";
                    else
                        std::cerr << ".  ";
                }
                std::cerr << std::endl;
                if (i == MAX - 1) {
                    std::cerr << " ";
                    for (int j = 0; j < MAX; ++j) {
                        if (j < 10)
                            std::cerr << "  " << j;
                        else
                            std::cerr << " " << j;
                    }
                    std::cerr << std::endl;
                }
            }
        }
#endif
    }

    // Pre: p0 conté una posició vàlida del mapa
    // Post: Calcula un bfs petit per sortir de sota l'àrea d'un helicòpter
    void cEvitaHelis(VVPa &M, const Posicio &p0) {
        std::queue<P2> Q;
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

        while (!Q.empty()) {
            // Obtenim el primer element
            P2 p = Q.front();
            Q.pop();

            if (!V[p.second.x][p.second.y]) {
                V[p.second.x][p.second.y] = true;
                Posicio pn(p0.x + p.second.x, p0.y + p.second.y);

                // Primer mirem si es pot passar per la posició actual
                if (!passar(pn))
                    M[pn.x][pn.y] = PS(NO_PASSAR, NO_PASSAR);
                // Si es pot passar comprovem que sigui una casella del voltant
                else if (p.first == POST) {
                    for (int i = 0; i < DIRS; ++i) {
                        Posicio pos(p.second.x + X[i], p.second.y + Y[i]);
                        if (pos.x >= 0 && pos.x < 7 && pos.y >= 0 && pos.y < 7) {
                            Q.push(P2(INV[i], pos));
                        }
                    }
                }
                // NO és una casella del voltant
                else {
                    M[pn.x][pn.y] = PS(p.first, NO_PASSAR);
                    for (int i = 0; i < DIRS; ++i) {
                        Posicio pos(p.second.x + X[i], p.second.y + Y[i]);
                        Q.push(P2(INV[i], pos));
                    }
                }
            }
        }
    }

    // Pre: M és la matriu de posicions iniciada amb els valors per defecte
    // Post: afegeix les posicions dels helicòpters per per evitar passar-hi i
    // que matin soldats
    void evitaHelis(VVPa &M) {
        std::queue<P2> Q;
        // Hem de mirar tots els helicòpters del tauler per evitar-los tots
        for (int i = 1; i <= 4; ++i)
            if (player != i) {
                VE H = helis(i);
                for (int h : H) {
                    Info I = dades(h);
                    if (I.napalm < MAX_NAPALM) {
                        cEvitaHelis(M, Posicio(I.pos.x - 3, I.pos.y - 3));
                    }
                }
            }

#ifndef NDEBUG
        if (quin_torn() % 30 == 0) {
            std::cerr << "HELIS" << std::endl;
            for (int i = 0; i < MAX; ++i) {
                if (i == 0) {
                    std::cerr << " ";
                    for (int j = 0; j < MAX; ++j) {
                        if (j < 10)
                            std::cerr << "  " << j;
                        else
                            std::cerr << " " << j;
                    }
                    std::cerr << std::endl;
                }
                if (i < 10)
                    std::cerr << " " << i << "|";
                else
                    std::cerr << i << "|";
                for (int j = 0; j < MAX; ++j) {
                    if (M[i][j].first >= 0)
                        std::cerr << M[i][j].first << "  ";
                    else if (M[i][j].first == NO_PASSAR)
                        std::cerr << "N  ";
                    else
                        std::cerr << ".  ";
                }
                std::cerr << std::endl;
                if (i == MAX - 1) {
                    std::cerr << " ";
                    for (int j = 0; j < MAX; ++j) {
                        if (j < 10)
                            std::cerr << "  " << j;
                        else
                            std::cerr << " " << j;
                    }
                    std::cerr << std::endl;
                }
            }
        }
#endif
    }

    bool hiHaHelis(Posicio &p, const Info &hel, const VVE &H) {
        int h = H[p.x][p.y];
        if (h > 0 && h != hel.id)
            return true;

        for (int i = 0; i < DIRS; ++i) {
            h = H[p.x + X[i]][p.y + Y[i]];
            if (h > 0 && h != hel.id)
                return true;
        }
        for (int i = 0; i < DIRS2; ++i) {
            h = H[p.x + X2[i]][p.y + Y2[i]];
            if (h > 0 && h != hel.id)
                return true;
        }
        return false;
    }

    // Pre: cap
    // Post: totes les posicions agafables per un helicòpter estan marcades
    void cHiHaHelis(VVE &he) {
        for (int i = 1; i <= 4; ++i) {
            VE H = helis(i);
            for (int h : H) {
                Info dH = dades(h);
                he[dH.pos.x][dH.pos.y] = h;
                for (int j = 0; j < DIRS; ++j) {
                    he[dH.pos.x + X[j]][dH.pos.y + Y[j]] = h;
                }
                for (int j = 0; j < DIRS2; ++j) {
                    he[dH.pos.x + X2[j]][dH.pos.y + Y2[j]] = h;
                }
            }
        }
    }

    // Pre: p és una posició vàlida del maà
    // Post: retorna el càlcul de jugadors
    int heliAtac(const Posicio &p) {
        // Primer de tot busquem si es pot tirar NAPALM
        int totMeus = 0;
        int totEnem = 0;
        int s = quin_soldat(p);
        if (s > 0)
            (dades(s).equip != player) ? ++totEnem : ++totMeus;

        for (int i = 0; i < DIRS; ++i) {
            Posicio pos(p.x + X[i], p.y + Y[i]);
            s = quin_soldat(pos);
            if (s > 0)
                (dades(s).equip != player) ? ++totEnem : ++totMeus;
        }
        for (int i = 0; i < DIRS2; ++i) {
            Posicio pos(p.x + X2[i], p.y + Y2[i]);
            s = quin_soldat(pos);
            if (s > 0)
                (dades(s).equip != player) ? ++totEnem : ++totMeus;
        }
        return totEnem - totMeus;
    }

    // Pre: s conté un soldat vàlid al que se li poden donar ordres
    // Post: s ha atacat si s'ha donat la ocasió i ha atacat al que té més
    // probabilitats de matar
    bool solAtaca(const Info &s) {
        // ID del soldat a atacar
        int solAPo = 0;
        int solANe = 0;
        // Càlcul de la vida d'atac (VIDA - PROMIG_TERRENY). Abs(solMc) ha de
        // ser mínim
        int solPos = 1000;
        int solNeg = -1000;
        Posicio post(100, 100);
        bool postT = false;

        // Comprovem les 8 direccions
        for (int i = 0; i < DIRS; ++i) {
            // Calculem posició
            Posicio q(s.pos.x + X[i], s.pos.y + Y[i]);

            // Si hi ha un post al voltant l'intentem capturar però només si no
            // hem atacat
            int po = de_qui_post(q);
            if (po != 0 && po != player) {
                post = q;
                postT = true;
            }

            // Obtenim dades del soldat
            int sol = quin_soldat(q);
            Info dSol;
            // Si trobem un soldat al nostre voltant el guardem per mirar-ne la
            // vida
            if (sol && (dSol = dades(sol)).equip != player) {
                int aux;
                if (que(q) == BOSC)
                    aux = dSol.vida - BOSC_M;
                else
                    aux = dSol.vida - GESPA_M;
                if (aux <= 0) {
                    if (solANe == 0) {
                        solANe = sol;
                        solNeg = aux;
                    } else if (aux > solNeg) {
                        solANe = sol;
                        solNeg = aux;
                    }
                } else if (aux > 0) {
                    if (solAPo == 0) {
                        solAPo = sol;
                        solPos = aux;
                    } else if (aux < solPos) {
                        solAPo = sol;
                        solPos = aux;
                    }
                }
            }
        }
        if (solANe) {
            Info sM = dades(solANe);
            ordena_soldat(s.id, sM.pos.x, sM.pos.y);
            return true;
        }
        if (solAPo) {
            Info sM = dades(solAPo);
            ordena_soldat(s.id, sM.pos.x, sM.pos.y);
            return true;
        }
        if (postT) {
            ordena_soldat(s.id, post.x, post.y);
            return true;
        }
        return false;
    }

    // Pre: cap
    // Post: mou els soldats per tal d'atacar o explorar amb una proporcio
    // PROP_1 o PROP_2 en funció del torn que sigui
    void jugaSoldats(VE &soldats) {
        for (int a : soldats) {
            // Assignem o comprovem la personalitat
            MI per = pers.find(a);
            if (per == pers.end()) {
                short personality;
                if (torns_restants() <= T_R_CANVI) {
                    personality = (probabilitat(PROP_2) ? ATACANT : EXPLORADOR);
                } else {
                    personality = (probabilitat(PROP_1) ? ATACANT : EXPLORADOR);
                }
                per = pers.insert(per, MES::value_type(a, personality));
            }

            // Obtenim les dades del soldat per passar a la funció ataca
            Info s = dades(a);

            // Reassignació del rol en funció de la vida, enviem kamikaze a
            // explorar
            if (s.vida < MIN_VIDA)
                pers[a] = ATACANT;

            // Guardem la personalitat
            int person = per->second;

            // Si hem conquerit totes les bases canviem el rol a un més agressiu
            if (totsConq || dir[s.pos.x][s.pos.y].first < 0)
                person = ATACANT;

            // Ens dirà si ja ha atact per no fer crides inútils. Crida a la
            // funció que mira al voltant i ataca també mira si hi ha post al
            // voltant i els conquereix per si de cas
            bool atacat = solAtaca(s);

            // En funció de la personalitat fem una cosa o una altra
            // Personalitat atacant, intenta matar a tants soldats com pot
            if (!atacat && person == ATACANT) {
                if (atac2[s.pos.x][s.pos.y].first >= 0) {
                    int x = s.pos.x;
                    int y = s.pos.y;
                    s.pos.x += X[atac2[x][y].first];
                    s.pos.y += Y[atac2[x][y].first];

                    ordena_soldat(a, s.pos.x, s.pos.y);
                } else if (atac[s.pos.x][s.pos.y].first >= 0) {
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

    // Pre: 'h' és l'identificador d'un helicòpter de l'equip
    // Post: Mira quina és la millor opció per l'helicòpter fent un Dijkstra
    int calculaH(const Info &h, VVE &dens, VVE &hi, bool tirat) {

        // Primer de tot busquem si es pot tirar NAPALM
        if (dens[h.pos.x][h.pos.y] < 0) {
            dens[h.pos.x][h.pos.y] = heliAtac(h.pos);
        }
        if (dens[h.pos.x][h.pos.y] >= 3 && h.napalm == 0 && !tirat) {
            return NAPALM;
        }

        if (de_qui_post(h.pos) != 0 && valor_post(h.pos.x, h.pos.y) == VALOR_ALT
            && torns_restants() <= T_R_CANVI)
            return RELLOTGE;

        // A partir d'aquí sabem que s'ha de moure l'helicòpter

        // Declarem els contenidors
        // first -> distància
        // second -> direcció
        VVE V(MAX, VE(MAX, 10000));
        std::priority_queue<find2> Q;

        // Introducció inicial

        for (int i = 0; i < 4; ++i) {
            Posicio p(h.pos.x + XH[i], h.pos.y + YH[i]);
            find2 f(p, i, i, 1, false);
            if (i != h.orientacio)
                ++f.dist;
            if ((i + 2) % 4 == h.orientacio)
                ++f.dist;
            if (pH[p.x][p.y] && !hiHaHelis(p, h, hi))
                Q.push(f);

            V[p.x][p.y] = f.dist;
        }
        for (int i = 0; i < 4; ++i) {
            Posicio p(h.pos.x + 2 * XH[i], h.pos.y + 2 * YH[i]);
            find2 f(p, i, i, 1, true);
            if (i != h.orientacio)
                ++f.dist;
            if ((i + 2) % 4 == h.orientacio)
                ++f.dist;
            if (pH[p.x][p.y] && !hiHaHelis(p, h, hi))
                Q.push(f);

            V[p.x][p.y] = f.dist;
        }

        while (!Q.empty()) {
            find2 f0 = Q.top();
            Q.pop();

            if (dens[f0.pos.x][f0.pos.y] < 0) {
                dens[f0.pos.x][f0.pos.y] = heliAtac(f0.pos);
            }
            int post = de_qui_post(f0.pos);
            int sol = quin_soldat(f0.pos);
            if (torns_restants() <= T_R_CANVI) {
                if (post != 0 && valor_post(f0.pos.x, f0.pos.y) == VALOR_ALT) {
                    if (f0.dir0 == h.orientacio) {
                        return (f0.ava) ? AVANCA2 : AVANCA1;
                    }
                    if ((f0.dir0 + 1) % 4 == h.orientacio)
                        return RELLOTGE;
                    return CONTRA_RELLOTGE;
                }
            } else if (dens[f0.pos.x][f0.pos.y] >= 2) {
                if (f0.dir0 == h.orientacio) {
                    return (f0.ava) ? AVANCA2 : AVANCA1;
                }
                if ((f0.dir0 + 1) % 4 == h.orientacio)
                    return RELLOTGE;
                return CONTRA_RELLOTGE;
            }

            find2 aux = f0;
            for (int i = 0; i < 4; ++i) {
                Posicio p(f0.pos.x + XH[i], f0.pos.y + YH[i]);
                aux.pos = p;
                aux.dir = i;
                aux.dist = f0.dist + 1;

                if (i != f0.dir)
                    ++aux.dist;
                if ((i + 2) % 4 == f0.dir)
                    ++aux.dist;

                if (pH[p.x][p.y] && !hiHaHelis(p, h, hi) && V[p.x][p.y] > aux.dist) {
                    V[p.x][p.y] = aux.dist;
                    Q.push(aux);
                }
            }
            for (int i = 0; i < 4; ++i) {
                Posicio p(f0.pos.x + 2 * XH[i], f0.pos.y + 2 * YH[i]);
                aux.pos = p;
                aux.dir = i;
                aux.dist = f0.dist + 1;

                if (i != f0.dir)
                    ++aux.dist;
                if ((i + 2) % 4 == f0.dir)
                    ++aux.dist;
                if (pH[p.x][p.y] && !hiHaHelis(p, h, hi) && V[p.x][p.y] > aux.dist) {
                    V[p.x][p.y] = aux.dist;
                    Q.push(aux);
                }
            }
        }
        int un = uniforme(1, 5);
        return un;
    }

    // Pre: H és l'identificador d'un helicòpter de l'equip
    // Post: L'helicòpter mira si pot tirar paracaigudes i si pot els tira
    int tiraParaques(Info &h) {
        unsigned short tirats = 0;
        if ((torns_restants() <= T_R_CANVI && h.paraca.size() > 3)
            || (h.paraca.size() && torns_restants() > T_R_CANVI)
            || (torns_restants() < 10 && h.paraca.size())) {
            // Per defecte no hem trobat cap lloc on tirar-ne un
            // Anem de 0 a 2 pels 3 nivells de profunditat als que pot
            // accedir l'helicòpter
            for (int i = 0; i < 2 && tirats <= MAX_BAIXEN && tirats < h.paraca.size(); ++i) {

                // Mirem per totes les direccions del voltant
                for (int j = 0; j < DIRS && tirats < MAX_BAIXEN && tirats < h.paraca.size(); ++j) {
                    Posicio q(h.pos.x + i * X[j], h.pos.y + i * Y[j]);
                    if (pA[q.x][q.y]) {
                        // Mirem si es compleixen les condicions per llençar
                        // un soldat i el tirem
                        if (que(q.x, q.y) != AIGUA && quin_soldat(q) == 0) {
                            ordena_paracaigudista(q.x, q.y);
                            ++tirats;
                        }
                    }
                }
            }
            for (int i = 0; i < DIRS && tirats <= MAX_BAIXEN && tirats < h.paraca.size(); ++i) {
                Posicio q(h.pos.x + X2[i], h.pos.y + Y2[i]);
                if (pA[q.x][q.y]) {
                    if (que(q) != AIGUA && quin_soldat(q) == 0) {
                        ordena_paracaigudista(q.x, q.y);
                        ++tirats;
                    }
                }
            }
        } else if (h.paraca.size() && h.paraca[0] < 4) {
            bool found = false;
            for (int i = 0; i < 2 && !found; ++i) {

                // Mirem per totes les direccions del voltant
                for (int j = 0; j < DIRS && !found; ++j) {
                    Posicio q(h.pos.x + i * X[j], h.pos.y + i * Y[j]);
                    if (pA[q.x][q.y]) {
                        // Mirem si es compleixen les condicions per llençar
                        // un soldat i el tirem
                        if (que(q.x, q.y) != AIGUA && quin_soldat(q) == 0) {
                            ordena_paracaigudista(q.x, q.y);
                            ++tirats;
                            found = true;
                        }
                    }
                }
            }
            for (int i = 0; i < DIRS && !found; ++i) {
                Posicio q(h.pos.x + X2[i], h.pos.y + Y2[i]);
                if (pA[q.x][q.y]) {
                    if (que(q) != AIGUA && quin_soldat(q) == 0) {
                        ordena_paracaigudista(q.x, q.y);
                        ++tirats;
                        found = true;
                    }
                }
            }
        }
        return tirats;
    }

    // Pre: VE és un std::vector d'enters que conté els ID dels helicòpters;
    // Post: Mira si l'helicòpter té lloc per tirar els paracaigudistes i també
    // mira els moviments que ha de fer l'helicòpter
    void jugaHelis(VE &helis) {
        VVE dens(MAX, VE(MAX, -1));
        VVE hiHelis(MAX, VE(MAX, 0));
        cHiHaHelis(hiHelis);
        for (int i = 0; i < MAX; ++i) {
            for (int j = 0; j < MAX; ++j) {
                if (pH[i][j])
                    dens[i][j] = heliAtac(Posicio(i, j));
            }
        }

        for (int a : helis) {
            int paraques = 0;
            Info h = dades(a);
            if (paraques < MAX_BAIXEN)
                paraques += tiraParaques(h);
            int ordre = calculaH(h, dens, hiHelis, paraques != 0);
            if (ordre > 0)
                ordena_helicopter(a, ordre);
        }
    }

    // Pre: M és una matriu inicialitzada amb els valors per defecte, type conté
    // el tipus indicat (ATAC o POST) per indicar el que s'ha de calcular
    // Post: calcula el camp vectorial per poder arribar als objectius indicats
    void updateBFS(int type, VVPa &M) {
        // Reiniciem la matriu del BFS
        M = VVPa(MAX, VPa(MAX, PS(NO_VIST, NO_VIST)));
        evitaHelis(M);      // Cridem la funció que evita els helicòpters
        std::queue<find> Q; // Cua on guardem totes les dades a buscar

        // En funció del tipus afegim uns valors o uns altres
        if (type == POST) {
            VP2 Pos = posts();
            totsConq = true;
            for (Post y : Pos) {
                if (y.equip != player && M[y.pos.x][y.pos.y].second != NO_PASSAR) {
                    Q.push(find(POST, 0, y.pos));
                    totsConq = false;
                } else if (int(soldats(player).size()) > MAX_SOLDATS) {
                    M[y.pos.x][y.pos.y] = PS(POST, 0);
                }
            }
        } else if (type == ENEM || type == ENEM2) {
            for (int i = 1; i <= 4; ++i) {
                if (i != player) {
                    VE V = soldats(i);
                    for (int a : V)
                        Q.push(find(ENEM, 0, dades(a).pos));
                }
            }
        } else
            std::cerr << "ERROR: Tipus mal escollit" << std::endl;

        while (!Q.empty()) {
            // Obtenim el primer
            find p = Q.front();
            Q.pop();

            // Es mira la primera posició de la cua ('p') i es comproven els
            // diferents casos
            if (M[p.pos.x][p.pos.y].first == NO_VIST) {

                // Mirem que no hi hagi cap obstacle
                if (!passar(p.pos))
                    M[p.pos.x][p.pos.y] = PS(NO_PASSAR, NO_PASSAR);
                // Es comprova també si és un objectiu a trobar
                else if (p.dir == type) {
                    M[p.pos.x][p.pos.y] = PS(type, 0);

                    // Mirem per distància 1 a tot el voltant per evitar
                    // passar per allà on ja hi ha soldats nostres
                    for (int i = 0; i < DIRS; ++i) {
                        // Calculem la posicio
                        Posicio q(p.pos.x + X[i], p.pos.y + Y[i]);
                        if (type == POST)
                            Q.push(find(INV[i], p.dist + 1, q));
                        else if (type == ENEM) {
                            // Obtenim el soldat a la posició 'q'
                            int sol = quin_soldat(q);
                            if (sol == 0 || dades(sol).equip != player) {
                                Q.push(find(INV[i], p.dist + 1, q));
                            }
                        }
                    }
                }
                // Si no és cap de les altres dues coses és una casella normal
                // de bosc o gespa
                else {
                    M[p.pos.x][p.pos.y] = PS(p.dir, p.dist);
                    for (int i = 0; i < DIRS; ++i) {
                        // Calculem la posició
                        Posicio q(p.pos.x + X[i], p.pos.y + Y[i]);
                        if (type == ENEM2) {
                            int sol = quin_soldat(p.pos);
                            if (sol == 0 || dades(sol).equip != player) {
                                Q.push(find(INV[i], p.dist + 1, q));
                            }
                        } else
                            Q.push(find(INV[i], p.dist + 1, q));
                    }
                }
            } else if (M[p.pos.x][p.pos.y].second > p.dist) {
                M[p.pos.x][p.pos.y] = PS(p.dir, p.dist);
            }
        }

#ifdef DDEBUG
        if (quin_torn() % 30 == 0) {
            if (type == POST)
                std::cerr << "EXPLORADORS" << std::endl;
            else
                std::cerr << "ATACANTS" << std::endl;
            for (int i = 0; i < MAX; ++i) {
                if (i == 0) {
                    std::cerr << " ";
                    for (int j = 0; j < MAX; ++j) {
                        if (j < 10)
                            std::cerr << "  " << j;
                        else
                            std::cerr << " " << j;
                    }
                    std::cerr << std::endl;
                }
                if (i < 10)
                    std::cerr << " " << i << "|";
                else
                    std::cerr << i << "|";
                for (int j = 0; j < MAX; ++j) {
                    if (M[i][j].first >= 0)
                        std::cerr << M[i][j].first << "  ";
                    else if (M[i][j].first == NO_PASSAR)
                        std::cerr << ".  ";
                    else if (M[i][j].first == type)
                        std::cerr << "#  ";
                    else
                        std::cerr << ".  ";
                }
                std::cerr << std::endl;
                if (i == MAX - 1) {
                    std::cerr << " ";
                    for (int j = 0; j < MAX; ++j) {
                        if (j < 10)
                            std::cerr << "  " << j;
                        else
                            std::cerr << " " << j;
                    }
                    std::cerr << std::endl;
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
    virtual void play() {
        // Valors per torns especifics
        if (quin_torn() == 0) {
            precalculaH();
            player = qui_soc();
            totsConq = false;
        } else if (torns_restants() == T_R_CANVI) {

            // Canviem totes les proporcions quan queden menys de 50 torns,
            // aquí se suposa que haurem aconseguit suficients soldats com per
            // anar a full
            for (std::pair<const int, short> &a : pers)
                a.second = (probabilitat(PROP_2) ? ATACANT : EXPLORADOR);
        }

        // Actualitzem les matrius per buscar camins
        updateBFS(POST, dir);
        updateBFS(ENEM, atac);
        updateBFS(ENEM2, atac2);

        // Donem les ordres dels paracaigudes
        VE h = helis(player);
        jugaHelis(h);

        // Donem les ordres dels soldats
        VE s = soldats(player);
        jugaSoldats(s);
    }
};

/**
 * No toqueu aquesta línia.
 */
RegisterPlayer(PLAYER_NAME);
