
#include "Player.hh"

using namespace std;


/**
 * Escriu el nom * del teu jugador i guarda
 * aquest fitxer amb el nom AI*.cc
 */
#define PLAYER_NAME jormaig


/**
 * Podeu declarar constants aquí
 */
const int NO_PASSAR = -1;   // Ens indica que no hem de passar
const int NO_VIST   = -2;   // Pel BFS, per marcar les caselles no vistes
const int POST      = -3;   // Ens indica que hi ha un post
const int MAX_FOC   =  2;   // Màxim de torns que tolerem un foc per un BFS
const int DIRS      =  8;   // Direccions totals en les que es pot avançar

const int X[8] = { 1, 1, 0, -1, -1, -1,  0,  1 };
const int Y[8] = { 0, 1, 1,  1,  0, -1, -1, -1 };
const int INV[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };

typedef vector< P > VPa;
typedef vector< VPa > VVPa;


struct PLAYER_NAME : public Player
{


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
    
    
    struct find
    {
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
    
    int player;
    
    // first -> direcció
    // second -> distància
    // Si és un post o una posició no vàlida tenim POST i NO_PASSAR
    VVPa dir;
    
    void atacaVoltant(VE &soldats)
    {
        for (int y : soldats)
        {
            Info s = dades(y);
            bool found = false;
            
            for (int i = 0; i < DIRS and not found; ++i)
            {
                int sol = quin_soldat(s.pos.x + X[i], s.pos.y + Y[i]);
                Info dSol;
                
                if (sol and (dSol = dades(sol)).equip != player)
                {
                    found = true;
                    ordena_soldat(y, dSol.pos.x, dSol.pos.y);
                }
            }
        }
    }
    
    void mouSoldats(VE &soldats)
    {
        for (int a : soldats)
        {
            Info s = dades(a);
            
            if (dir[s.pos.x][s.pos.y].first >= 0)
            {
                int x = s.pos.x;
                int y = s.pos.y;
                     
                s.pos.x += X[dir[x][y].first];
                s.pos.y += Y[dir[x][y].first];
                ordena_soldat(a, s.pos.x, s.pos.y);
            }
        }
    }
    
    void updatePosts()
    {
        queue< find > Q;
        VP2 Pos = posts();
        
        for (Post y : Pos)
            if (y.equip != player) Q.push(find(POST, 0, y.pos));
            
        while (not Q.empty())
        {
            find p = Q.front();
            Q.pop();
            
            if (dir[p.pos.x][p.pos.y].first == NO_VIST)
            {
                int que = Board::que(p.pos.x, p.pos.y);
                
                if (que == MUNTANYA or que == AIGUA)
                {
                    dir[p.pos.x][p.pos.y] = make_pair(NO_PASSAR, NO_PASSAR);
                }
                else
                {
                    if (p.dir == POST)
                        dir[p.pos.x][p.pos.y] = make_pair(POST, 0);
                    else dir[p.pos.x][p.pos.y] = make_pair(p.dir, p.dist);
                    
                    for (int i = 0; i < DIRS; ++i)
                    {
                        Q.push(find(INV[i], p.dist + 1,
                                    Posicio(p.pos.x + X[i], p.pos.y + Y[i])));
                    }
                }
            }
        }        
    }
    
    /**
     * Mètode play.
     *
     * Aquest mètode serà invocat una vegada cada torn.
     */
    virtual void play ()
    {
        if (quin_torn() == 0)
        {
            player = qui_soc();
            dir = VVPa(MAX, VPa(MAX, make_pair(NO_VIST, NO_VIST)));
            updatePosts();
        }
        else if (quin_torn()%5 == 0)
        {
            dir = VVPa(MAX, VPa(MAX, make_pair(NO_VIST, NO_VIST)));
            updatePosts();
        }
        
        VE s = soldats(player);
        atacaVoltant(s);
        mouSoldats(s);
        
    }
    
    
};


/**
 * No toqueu aquesta línia.
 */
RegisterPlayer(PLAYER_NAME);

