
#include "player.hpp"



/**
 * Escriu el nom * del teu jugador i guarda
 * aquest fitxer amb el nom AI*.cc
 */
#define PLAYER_NAME Demo

/**
 * Podeu declarar constants aquí
 */

const int CAOTIC = 1;
const int CICLIC = 2;

const int X[8] = {1, 1, 0, -1, -1, -1, 0, 1};
const int Y[8] = {0, 1, 1, 1, 0, -1, -1, -1};

struct PLAYER_NAME : public Player {

    /**
     * Factory: retorna una nova instància d'aquesta classe.
     * No toqueu aquesta funció.
     */
    static Player *factory() { return new PLAYER_NAME; }

    /**
     * Els atributs dels vostres jugadors es poden definir aquí.
     */

    typedef std::vector<int> VE;
    typedef std::map<int, int> MEE;
    typedef MEE::iterator MI;

    MEE personalitat; // Guarda la personalitat dels soldats entre torns.

    void juga_soldat(int equip, int id) {
        MI p = personalitat.find(id);
        // Els soldats que encara no estan decidits, els fem cíclics
        // amb probabilitat 0.4, i caòtics altrament.
        if (p == personalitat.end())
            personalitat[id] = (probabilitat(0.4) ? CICLIC : CAOTIC);
        int pers = personalitat[id];

        Info in = dades(id);
        int x = in.pos.x;
        int y = in.pos.y;
        for (int i = 0; i < 8; ++i) {
            int xx = x + X[i];
            int yy = y + Y[i];
            if (xx >= 0 && xx < MAX && yy >= 0 && yy < MAX) {
                int id2 = quin_soldat(xx, yy);
                // Si tenim un enemic al costat, l'ataquem.
                if (id2 && dades(id2).equip != equip) {
                    ordena_soldat(id, xx, yy);
                    return;
                }
            }
        }

        int xx, yy;
        if (pers == CICLIC) {
            int torn = quin_torn() / 5;
            int incx = torn % 4 - 1;
            if (incx == 2)
                incx = 0;
            int incy = (torn + 1) % 4 - 1;
            if (incy == 2)
                incy = 0;
            xx = x + incx;
            yy = y + incy;
        } else {
            xx = x + uniforme(-1, 1);
            yy = y + uniforme(-1, 1);
        }
        if (xx >= 0 && xx < MAX && yy >= 0 && yy < MAX)
            ordena_soldat(id, xx, yy);
    }

    void juga_heli(int equip, int id) {
        Info in = dades(id);
        int x = in.pos.x;
        int y = in.pos.y;
        int suma = 0;
        for (int i = -ABAST; i <= ABAST; ++i)
            for (int j = -ABAST; j <= ABAST; ++j) {
                int xx = x + i;
                int yy = y + j;
                if (xx >= 0 && xx < MAX && yy >= 0 && yy < MAX) {
                    int id2 = quin_soldat(xx, yy);
                    if (id2 && dades(id2).equip != equip)
                        ++suma;
                }
            }
        // Si tenim 3 o més enemics a sota, intentem llençar napalm.
        if (suma >= 3 && dades(id).napalm == 0) {
            ordena_helicopter(id, NAPALM);
            return;
        }

        // Amb probabilitat 20% girem en el sentit contrari de les agulles del rellotge,
        // altrament intentem avançar dos passos.
        int c = uniforme(1, 5);
        ordena_helicopter(id, c == 1 ? CONTRA_RELLOTGE : AVANCA2);
    }

    void llenca_paracaigudista(int heli_id) {
        // Obtenim les dades de l'helicòpter...
        Info in = dades(heli_id);
        // ... i intentem llencar un paracaigudista, sense ni mirar el terreny.
        int xx = in.pos.x + uniforme(-2, 2);
        int yy = in.pos.y + uniforme(-2, 2);
        if (xx >= 0 && xx < MAX && yy >= 0 && yy < MAX)
            ordena_paracaigudista(xx, yy);
    }

    /**
     * Mètode play.
     *
     * Aquest mètode serà invocat una vegada cada torn.
     */
    virtual void play() {

        int equip = qui_soc();
        VE H = helis(equip);   // helicòpters del meu equip
        VE S = soldats(equip); // soldats del meu equip

        // si en un helicòpter escollit a l'atzar tinc paracaigudistes, en llenço un.
        int heli_id = H[uniforme(0, H.size() - 1)];
        if (!dades(heli_id).paraca.empty())
            llenca_paracaigudista(heli_id);

        for (int i = 0; i < (int)H.size(); ++i)
            juga_heli(equip, H[i]);
        for (int i = 0; i < (int)S.size(); ++i)
            juga_soldat(equip, S[i]);
    }
};

/**
 * No toqueu aquesta línia.
 */
RegisterPlayer(PLAYER_NAME);
