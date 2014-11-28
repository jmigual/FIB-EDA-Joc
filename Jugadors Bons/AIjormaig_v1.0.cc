
#include "Player.hh"

using namespace std;


/**
 * Escriu el nom * del teu jugador i guarda
 * aquest fitxer amb el nom AI*.cc
 */
#define PLAYER_NAME jormaig_v1_0


/**
 * Podeu declarar constants aquí
 */
const int NO_PASSAR = -1;   // Ens indica que no hem de passar
const int NO_VIST   = -2;   // Pel BFS, per marcar les caselles no vistes
const int POST      = -3;   // Ens indica que hi ha un post
const int VOLTANT   = -4;

const int MAX_FOC   =  1;   // Màxim de torns que tolerem un foc per un BFS
const int DIRS      =  8;   // Direccions totals en les que es pot avançar

const int CALCUL_EXPL = 1;  // Nombre de torns entre els quals es recalcula el
// BFS de l'exploradors


// PERSONALITATS
const int ATACANT       = 1;
const int EXPLORADOR    = 2;
const double PROPORCIO_1  = 0.9;
const double PROPORCIO_2  = 0.4;

const int X[8] = { 1, 1, 0, -1, -1, -1,  0,  1 };
const int Y[8] = { 0, 1, 1,  1,  0, -1, -1, -1 };
const int INV[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };


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
    
    typedef vector< P > VPa;
    typedef vector< VPa > VVPa;
    typedef map<int, int> MEE;
    typedef MEE::iterator MI;
    typedef pair<int, Posicio> PEP;
    
    struct find {
        // Direcció
        int dir;
        // Distància
        int dist;
        // Posició
        Posicio pos;
        
        find(int a, int b, Posicio pos) : dir(a), dist(b), pos(pos) {}
        
        static find makeFind(int a, int b, const Posicio &pos)
        {
            return find(a, b, pos);
        }
    };
    
    // Número de jugador, es guarda a la primera ronda
    int player;
    
    // Guarda informació de com podem anar als POST
    // first -> direcció
    // second -> distància
    // Si és un post o una posició no vàlida tenim POST i NO_PASSAR
    VVPa dir;
    
    // Guarda informació dels soldats enemics més propers
    VVE atac;
    
    // Guarda les personalitats dels soldats
    MEE pers;
    
    
    // Pre: cap
    // Post: retorna TRUE si a la posició p no hi ha cap condició que impedeixi
    // passar-hi
    bool passar(Posicio &p)
    {
        // Obtenim el tipus d'element
        int que = Board::que(p.x, p.y);
        
        return que != MUNTANYA and que != AIGUA and
               temps_foc(p.x, p.y) < MAX_FOC;
    }
    
    
    // Pre: cap
    // Post: mou els soldats per tal d'atacar o explorar amb una proporcio
    // PROPORCIO_1
    void jugaSoldats(VE &soldats)
    {
        for (int a: soldats) {
            // Assignem o comprovem la personalitat
            MI per = pers.find(a);
            if (per == pers.end())
                pers[a] = (probabilitat(PROPORCIO_1) ? ATACANT : EXPLORADOR);
            int person = pers[a];
            
            // Primer comprovem si podem atacar a algú al voltant
            Info s = dades(a);
            bool found = false;
            // Comprovem les 8 direccions
            for (int i = 0; i < DIRS and not found; ++i) {
                int sol = quin_soldat(s.pos.x + X[i], s.pos.y + Y[i]);
                Info dSol;
                // Si trobem un soldat al nostre voltant l'ataquem
                if (sol and (dSol = dades(sol)).equip != player) {
                    found = true;
                    ordena_soldat(a, dSol.pos.x, dSol.pos.y);
                }
            }
            
            // En funció de la personalitat fem una cosa o una altra
            if (person == ATACANT) {
                if (atac[s.pos.x][s.pos.y] >= 0) {
                    int x = s.pos.x;
                    int y = s.pos.y;
                    s.pos.x += X[atac[x][y]];
                    s.pos.y += Y[atac[x][y]];
                    
                    ordena_soldat(a, s.pos.x, s.pos.y);
                }
            }
            else {
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
                bool found = false;
                for (int i = 0; i < 3 and not found; ++i) {
                    for (int j = 0; j < DIRS and not found; ++j) {
                        int x = h.pos.x + i*X[j];
                        int y = h.pos.y + i*Y[j];
                        
                        if (que(x, y) != AIGUA and quin_soldat(x, y) == 0) {
                            ordena_paracaigudista(x, y);
                            found = true;
                        }
                    }
                }
            }
        }
        cerr << "FI" << endl;
    }
    
    // Pre: cap
    // Post: calcula les direccions cap on s'ha d'anar per trobar un soldat
    // enemic
    void updateKill()
    {
        // Reiniciem la matriu d'atac
        atac = VVE(MAX, VE(MAX, NO_VIST));
        
        // Cua on guardem totes les dades a buscar
        queue< PEP > Q;
        for (int i = 1; i <= 4; ++i) {
            if (i != player) {
                VE V = soldats(i);
                for (int a : V) Q.push(PEP(POST, dades(a).pos));
                
                // Part per evitar els helicopters
                VE H = helis(i);
                for (int a : H) {
                    Info h = dades(a);
                    atac[h.pos.x][h.pos.y] = NO_PASSAR;
                    for (int j = 0; j < DIRS; ++j) {
                        atac[h.pos.x + X[j]][h.pos.y + Y[j]] = j;
                        atac[h.pos.x + 2*X[j]][h.pos.y + 2*Y[j]] = j;
                    }
                }
            }
        }
        
        
        while (not Q.empty()) {
            // Traiem i eliminem el primer element
            PEP p = Q.front();
            Q.pop();
            
            // Comprovem si ja hem visitat l'element
            if (atac[p.second.x][p.second.y] == NO_VIST) {
            
                // Busquem que no sigui un obstacle
                if (not passar(p.second)) {
                    atac[p.second.x][p.second.y] = NO_PASSAR;
                }
                else {
                    if (p.first == POST) {
                        atac[p.second.x][p.second.y] = POST;
                        for (int i = 0; i < DIRS; ++i) {
                            Posicio q(p.second.x + X[i], p.second.y + Y[i]);
                            int sol = quin_soldat(q.x, q.y);
                            if (sol) {
                                if (dades(sol).equip != player)
                                    Q.push(PEP(INV[i], q));
                            }
                            else Q.push(PEP(INV[i], q));
                        }
                    }
                    else {
                        atac[p.second.x][p.second.y] = p.first;
                        for (int i = 0; i < DIRS; ++i) {
                            Q.push(PEP(INV[i], Posicio(p.second.x + X[i],
                                                       p.second.y + Y[i])));
                        }
                    }
                }
            }
        }
    }
    
    void updatePosts()
    {
        dir = VVPa(MAX, VPa(MAX, make_pair(NO_VIST, NO_VIST)));
        
        // Cua on anirem guardant totes les dades a buscar
        queue< find > Q;
        VP2 Pos = posts();
        
        // Afegim tots els posts que no són del nostre equip a la cua per
        // buscar com arrivar-hi
        for (Post y : Pos) if (y.equip != player) Q.push(find(POST, 0, y.pos));
        
        for (int i = 1; i < 4; ++i) {
            if (i != player) {
                VE H = helis(i);
                for (int a : H) {
                    Info h = dades(a);
                    dir[h.pos.x][h.pos.y] = P(NO_PASSAR, NO_PASSAR);
                    for (int j = 0; j < DIRS; ++j) {
                        dir[h.pos.x + X[j]][h.pos.y + Y[j]] =
                            P(j, 10);
                        dir[h.pos.x + 2*X[j]][h.pos.y + 2*Y[j]] =
                            P(j, 10);
                    }
                }
            }
        }
        
        // Comencem a buscar
        while (not Q.empty()) {
            find p = Q.front();
            Q.pop();
            
            // Si no hem visitat la casella llavors hem de buscar-ne les dades
            if (dir[p.pos.x][p.pos.y].first == NO_VIST) {
                int que = Board::que(p.pos.x, p.pos.y);
                
                // Comprovem que no sigui un obstacle
                if (que == MUNTANYA or que == AIGUA or
                        temps_foc(p.pos.x, p.pos.y) >= MAX_FOC) {
                    dir[p.pos.x][p.pos.y] = P(NO_PASSAR, NO_PASSAR);
                }
                else {
                    // Aquí ja només pot ser POST, BOSC o GESPA
                    if (p.dir == POST)
                        dir[p.pos.x][p.pos.y] = P(POST, 0);
                    else dir[p.pos.x][p.pos.y] = P(p.dir, p.dist);
                    
                    // Per cada casella al voltant afegim
                    for (int i = 0; i < DIRS; ++i) {
                        Q.push(find(INV[i], p.dist + 1,
                                    Posicio(p.pos.x + X[i], p.pos.y + Y[i])));
                    }
                }
            }
        }
        if (quin_torn()%30 == 0)
            for (int i = 0; i < MAX; ++i) {
                if (i == 0 or i == MAX - 1) {
                    cerr << "   ";
                    for (int j = 0; j < MAX; ++j) {
                        if (j < 10) cerr << "  " << j;
                        else cerr << " " << j;
                    }
                    cerr << endl;
                }
                if (i < 10) cerr << " " << i << "|";
                else cerr << i << "|";
                for (int j = 0; j < MAX; ++j) {
                    if (dir[i][j].first >= 0) cerr << dir[i][j].first << "  ";
                    else if (dir[i][j].first == NO_PASSAR) cerr << "N  ";
                    else if (dir[i][j].first == POST) cerr << "P  ";
                    else cerr << "V ";
                }
                cerr << endl;
            }
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
        }
        else if (torns_restants() == 50) {
            for (pair<const int, int> &a : pers)
                a.second = (probabilitat(PROPORCIO_2) ? ATACANT : EXPLORADOR);
        }
        
        updatePosts();
        updateKill();
        
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

