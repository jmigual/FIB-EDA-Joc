#include "board.hpp"
#include "action.hpp"



const char Board::code[5] = {' ',
			     ':',  // BOSC
			     '.',  // GESPA
			     'O',  // AIGUA
			     'X'}; // MUNTANYA

void Board::ini_veins_en_creu() {
  int ix[4] = { 1, 0, -1, 0 };
  int iy[4] = { 0, 1, 0, -1 };
  int dx[4] = { 1, -1, -1, 1 };
  int dy[4] = { 1, 1, -1, -1 };

  Xcreu.clear();
  Ycreu.clear();
  Xvei.clear();
  Yvei.clear();
  Xdiag.clear();
  Ydiag.clear();

  for (int k = 0; k < 4; ++k) {
    Xcreu.push_back(ix[k]);
    Ycreu.push_back(iy[k]);
    Xvei.push_back(ix[k]);
    Yvei.push_back(iy[k]);
    Xdiag.push_back(dx[k]);
    Ydiag.push_back(dy[k]);
    Xvei.push_back(dx[k]);
    Yvei.push_back(dy[k]);
  }
  Xvei.push_back(0);
  Yvei.push_back(0);
}


// Executa l'ordre donada a un soldat de terra, comprovant que vagi a
// un lloc vàlid, que encara estigui viu, si entra dins del foc, i si
// hi ha un soldat a un vol anar. Si no, es mou i comprova si hi havia
// un post que encara no era del seu equip, i actualitza la informació
// coherentment. Si sí, i és d'un altre equip, calcula el mal que li
// fa en funció d'on estigui, i si els punts de vida arriben a zero el
// mata.
void Board::fes_torn_soldat(int id, int x2, int y2, std::vector<Action>& done) {

  if (x2 < 0 || x2 >= MAX || y2 < 0 || y2 >= MAX) return;
  if (terreny[x2][y2] != BOSC && terreny[x2][y2] != GESPA) return;

  MI p = dada.find(id);
  if (p == dada.end()) return;

  int equip = p->second.equip;
  if (foc[x2][y2]) {
    int nou_equip = diferent(equip);
    int heli_id = heli[nou_equip][A.uniforme(0, int(heli[nou_equip].size())-1)];
    dada[heli_id].paraca.push_back(TORNS_BAIXAR);
    mata_soldat(id);
    done[equip].ordres1[id] = Posicio(x2, y2);
    return;
  }

  int x = p->second.pos.x;
  int y = p->second.pos.y;
  int id2 = iden[SOLDAT][x2][y2];

  if (id2 == id) return;

  if (id2 == 0) {
    std::swap(iden[SOLDAT][x][y], iden[SOLDAT][x2][y2]);
    p->second.pos = Posicio(x2, y2);
    done[equip].ordres1[id] = Posicio(x2, y2);
    if (quipost[x2][y2] == 0 || quipost[x2][y2] == equip) return;

    quipost[x2][y2] = equip;
    int k = 0;
    while (post[k].pos != Posicio(x2, y2)) ++k;
    post[k].equip = equip;
    return;
  }

  MI q = dada.find(id2);
  int equip2 = q->second.equip;
  if (equip2 == equip) return;

  int a = (terreny[x2][y2] == BOSC ? DANY_BOSC : DANY_GESPA);
  q->second.vida -= A.uniforme(a, 2*a);
  if (q->second.vida > 0) return;

  int heli_id = heli[equip][A.uniforme(0, int(heli[equip].size())-1)];
  dada[heli_id].paraca.push_back(TORNS_BAIXAR);
  mata_soldat(id2);
  return;
}


// Executa totes les ordres donades a soldats en ordre aleatori.
void Board::fes_torn_soldats(std::vector<P2>& V, std::vector<Action>& done) {

  int n = V.size();
  VE perm = A.permutacio(n);
  for (int k = 0; k < n; ++k) {
    int z = perm[k];
    fes_torn_soldat(V[z].first, V[z].second.x, V[z].second.y, done);
  }
}


// Executa l'ordre donada a un soldat helicòpter. Si la instrucció és
// llançar napalm, cal comprovar que ja pugui fer-ho, marcar les
// caselles amb foc (el +1 és perquè fes_torn_heli() resta 1 a totes
// les caselles al final), matar els soldats que calgui, i
// assignar-los a l'equip que toqui. Si cal moure, es mira quants
// passos es demanaven (1 o 2) i quants pot donar, i es queda amb el
// mínim.
void Board::fes_torn_heli(int id, int ins, std::vector<Action>& done) {

  MI p = dada.find(id);
  int x = p->second.pos.x;
  int y = p->second.pos.y;
  int ori = p->second.orientacio;

  if (ins == CONTRA_RELLOTGE) {
    p->second.orientacio = (ori + 1)%4;
    done[p->second.equip].ordres2[id] = ins;
    return;
  }

  if (ins == RELLOTGE) {
    p->second.orientacio = (ori + 3)%4;
    done[p->second.equip].ordres2[id] = ins;
    return;
  }

  if (ins == NAPALM) {
    if (p->second.napalm) return;

    int equip = p->second.equip;
    for (int i = -ABAST; i <= ABAST; ++i)
      for (int j = -ABAST; j <= ABAST; ++j) {
	int xx = x + i;
	int yy = y + j;
	if (xx >= 0 && xx < MAX && yy >= 0 && yy < MAX) {
	  int t = terreny[xx][yy];
	  if (t != MUNTANYA) {
	    foc[xx][yy] = 1 + (t == BOSC ? CREMA_BOSC : CREMA_ALTRE);
	    int id2 = iden[SOLDAT][xx][yy];
	    if (id2) {
	      int equip2 = dada[id2].equip;
	      if (equip2 != equip) {
		int heli_id = heli[equip][A.uniforme(0, int(heli[equip].size())-1)];
		dada[heli_id].paraca.push_back(TORNS_BAIXAR);
	      }
	      else {
		int nou_equip = diferent(equip);
		int heli_id = heli[nou_equip][A.uniforme(0, int(heli[nou_equip].size())-1)];
		dada[heli_id].paraca.push_back(TORNS_BAIXAR);
	      }
	      mata_soldat(id2);
	    }
	  }
	}
      }
    p->second.napalm = TORNS_NAPALM + 1;
    done[p->second.equip].ordres2[id] = ins;
    return;
  }

  bool ok = true;
  int i = 2;
  int fi = (ins == AVANCA1 ? 3 : 4);
  while (ok && ++i <= fi) {
    for (int j = -2; ok && j <= 2; ++j) {
      int xx = x + i*Xcreu[ori] + j*Ycreu[ori];
      int yy = y + i*Ycreu[ori] + j*Xcreu[ori];
      if (terreny[xx][yy] == MUNTANYA || cel[xx][yy]) ok = false;
    }
  }
  int x2 = x + (i - 3)*Xcreu[ori];
  int y2 = y + (i - 3)*Ycreu[ori];
  p->second.pos.x = x2;
  p->second.pos.y = y2;
  desmarca_heli(x, y);
  marca_heli(x2, y2);
  iden[HELI][x][y] = 0;
  iden[HELI][x2][y2] = id;
  if      (i ==3)  done[p->second.equip].ordres2[id] = AVANCA1;
  else if (i == 4) done[p->second.equip].ordres2[id] = AVANCA2; 
}


// Executa totes les ordres donades a helicòpters en ordre aleatori.
// També decrementa tots els comptadors de napalm.
void Board::fes_torn_helis(std::vector<P>& V, std::vector<Action>& done) {

  int n = V.size();
  VE perm = A.permutacio(n);
  for (int k = 0; k < n; ++k) {
    int z = perm[k];
    int id = V[z].first;
    fes_torn_heli(id, V[z].second, done);
  }

  for (int e = 1; e <= 4; ++e)
    for (int k = 0; k < (int)heli[e].size(); ++k) {
      MI p = dada.find(heli[e][k]);
      if (p->second.napalm) --p->second.napalm;
    }
}


// Executa totes les ordres donades als paracaigudistes. No cal fer-ho
// en ordre aleatori perquè no hi ha interferències possibles. Comença
// decrementant els comptadors dels paracaigudistes que no baixen
// durant aquest torn (i matant els que ja han caducat). Després, es
// comprova que la casella a la que es vol baixar sigui de bosc o
// gespa, que no tingui foc, que no hi hagi ningú, i que estigui a
// sota d'un del dos helicòpters de l'equip. Si això falla, el
// paracaigudista mor i passa a un altre equip. En cas de baixar, es
// mira si es marca un post directament.
void Board::paracaigudistes_baixen(const VVP& ordres3, std::vector<Action>& done) {

  VE nous_para(5, 0);
  for (int e = 1; e <= 4; ++e) {
    for (int j = 0; j < (int)heli[e].size(); ++j) {
      int heli_id = heli[e][j];
      int x2 = dada[heli_id].pos.x;
      int y2 = dada[heli_id].pos.y;

      int cnt = 0;
      for (int k = 0; k < int(ordres3[e].size()); ++k) {
	int x = ordres3[e][k].x;
	int y = ordres3[e][k].y;
	if (abs(x2 - x) <= 2 && abs(y2 - y) <= 2)
	  ++cnt;
      }
      VE B;
      while (cnt < int(dada[heli_id].paraca.size())) {
	int a = dada[heli_id].paraca[cnt] - 1;
	if (a) B.push_back(a);
	else ++nous_para[diferent(e)];
	++cnt;
      }
      swap(dada[heli_id].paraca, B);
    }
  }

  for (int e = 1; e <= 4; ++e)
    for (int r = 0; r < nous_para[e]; ++r) {
      int heli_id = heli[e][A.uniforme(0, int(heli[e].size())-1)];
      dada[heli_id].paraca.push_back(TORNS_BAIXAR);
    }


  for (int e = 1; e <= 4; ++e)
    for (int i = 0; i < (int)ordres3[e].size(); ++i) {
      int x = ordres3[e][i].x;
      int y = ordres3[e][i].y;
      if (x < 0 || x >= MAX || y < 0 || y >= MAX) {
	int nou_equip = diferent(e);
	int heli_id = heli[nou_equip][A.uniforme(0, int(heli[nou_equip].size())-1)];
	dada[heli_id].paraca.push_back(TORNS_BAIXAR);
      }
      else {
  	int t = terreny[x][y];
  	int id2 = iden[SOLDAT][x][y];
  	int e2 = (id2 ? dada[id2].equip : 0);
  	if ((t != BOSC && t != GESPA) || foc[x][y] || (e2 && e2 == e)) {
	  int nou_equip = diferent(e);
	  int heli_id = heli[nou_equip][A.uniforme(0, int(heli[nou_equip].size())-1)];
	  dada[heli_id].paraca.push_back(TORNS_BAIXAR);
	}
  	else if (e2) {
	  int heli_id = heli[e2][A.uniforme(0, int(heli[e2].size())-1)];
	  dada[heli_id].paraca.push_back(TORNS_BAIXAR);
	}
  	else {
  	  bool ok = false;
  	  for (int j = 0; ! ok && j < (int)heli[e].size(); ++j) {
  	    int id = heli[e][j];
  	    int x2 = dada[id].pos.x;
  	    int y2 = dada[id].pos.y;
  	    if (abs(x2 - x) <= 2 && abs(y2 - y) <= 2) ok = true;
  	  }

  	  if (ok) {
  	    soldat[e].push_back(nou_soldat(e, x, y));
	    done[e].ordres3.push_back(Posicio(x, y));
  	    if (quipost[x][y] && quipost[x][y] != e) {
  	      quipost[x][y] = e;
  	      int k = 0;
  	      while (post[k].pos != Posicio(x, y)) ++k;
  	      post[k].equip = e;
  	    }
  	  }
  	  else {
	    int nou_equip = diferent(e);
	    int heli_id = heli[nou_equip][A.uniforme(0, int(heli[nou_equip].size())-1)];
	    dada[heli_id].paraca.push_back(TORNS_BAIXAR);
	  }
  	}
      }
    }
}


Board::Board(std::istream& is) {

  foo = -1;

  std::string s;
  is >> s; 
  if (s != "ApocalypseNow") {
    std::cerr << "Read std::string " << s << std::endl;
  }
  assert(s == "ApocalypseNow");
  is >> s; assert(s == "v2");

  names_ = std::vector<std::string>(4);
  is >> s; assert(s == "names");
  for (int pl = 0; pl < 4; ++pl)
    is >> names_[pl];

  is >> s;    assert(s == "round");
  is >> torn; assert(torn >= 0 && torn < TORNS_JOC);

  is >> s; assert(s == "nou");
  is >> nou;

  ini_veins_en_creu();

  A = atzar(seed + 43625 * torn);
  terreny = VVE(MAX, VE(MAX));
  foc = VVE(MAX, VE(MAX, 0));
  iden = VVVE(3, VVE(MAX, VE(MAX, 0)));
  cel = VVE(MAX, VE(MAX, false));
  quipost = VVE(MAX, VE(MAX, 0));
  valorpost = VVE(MAX, VE(MAX, 0));
  post = VP2(NUM_POSTS);

  at = VA(5);
  for (int i = 1; i <= 4; ++i) {
    at[i] = atzar(A.uniforme(1, 100000));
  }

  soldat  = VVE(5);
  heli    = VVE(5);

  is >> s;
  assert(s == "terreny");
  is >> s;
  // assert(s == "00000000001111111111222222222233333333334444444444555555555566666666667777777777");
  is >> s;
  // assert(s == "01234567890123456789012345678901234567890123456789012345678901234567890123456789");

  for (int i = 0; i < MAX; ++i) {
    int aux;
    is >> aux; assert(aux == i);
    for (int j = 0; j < MAX; ++j) {
      char c;
      is >> c;
      if      (c == code[BOSC    ]) terreny[i][j] = BOSC;
      else if (c == code[GESPA   ]) terreny[i][j] = GESPA;
      else if (c == code[AIGUA   ]) terreny[i][j] = AIGUA;
      else if (c == code[MUNTANYA]) terreny[i][j] = MUNTANYA;
    }
  }

  is >> s; assert(s == "posts");
  is >> s; assert(s == "equip");
  is >> s; assert(s == "x");
  is >> s; assert(s == "y");
  is >> s; assert(s == "valor");
  for (int k = 0; k < NUM_POSTS; ++k) {
    int equip, x, y, valor;
    is >> equip >> x >> y >> valor;
    assert(-1 == equip || ( 1 <= equip && equip <= 4));
    assert(valid(x, y));
    post[k].equip = equip;
    post[k].pos.x = x;
    post[k].pos.y = y;
    post[k].valor = valor;
    quipost[x][y] = equip;
    valorpost[x][y] = valor;
  }

  is >> s; assert(s == "score");
  std::vector<int> score(4);
  for (int pl = 0; pl < 4; ++pl)
    is >> score[pl];

  is >> s; assert(s == "status");
  status_ = std::vector<double>(4);
  for (int pl = 0; pl < 4; ++pl)
    is >> status_[pl];

  is >> s; assert(s == "agents");
  is >> s; assert(s == "id");
  is >> s; assert(s == "tipus");
  is >> s; assert(s == "equip");
  is >> s; assert(s == "x");
  is >> s; assert(s == "y");
  is >> s; assert(s == "vida");
  is >> s; assert(s == "ori");
  is >> s; assert(s == "napalm");
  is >> s; assert(s == "paraca");
  int id;

  while (is >> id && id != -1) {
    int tipus, equip, x, y, vida, ori, napalm, paraca_size;
    is >> tipus >> equip >> x >> y >> vida >> ori >> napalm >> paraca_size;
    VE par(paraca_size);
    for (int k = 0; k < paraca_size; ++k)
      is >> par[k];

    dada[id] = Info(id, tipus, equip, x, y, vida, ori, napalm, par);
    iden[tipus][x][y] = id;
    if (tipus == HELI) {
      for (int dx = -2; dx <= 2; ++dx)
	for (int dy = -2; dy <= 2; ++dy) {
	  int xx = x + dx;
	  int yy = y + dy;
	  if (valid(xx, yy)) cel[xx][yy] = 1;
	}
      heli[equip].push_back(id);
    }
    else soldat[equip].push_back(id);
  }

  is >> s; assert(s == "foc");
  is >> s; assert(s == "torns");
  is >> s; assert(s == "x");
  is >> s; assert(s == "y");
  int torns;
  while (is >> torns && torns != -1) {
    int x, y;
    is >> x >> y;
    assert(0 < torns);  // Només es guarden els no nuls.
    assert(valid(x, y));
    foc[x][y] = torns;
  }

  for (int pl = 0; pl < 4; ++pl) {
    assert(score[pl] == puntuacio(pl+1));
  }

  assert(ok());
}


void Board::print_header(std::ostream& out) const {

  out << "ApocalypseNow v2" << std::endl << std::endl;

  // Adding row numbers.
  out << "names" << std::endl;
  for (int e = 1; e <= 4; ++e)
    out << nom(e) << "\t";
  out << std::endl << std::endl;
}


void Board::print(std::ostream& out) const {

  out << "round " << quin_torn() << std::endl << std::endl;

  out << "nou" << std::endl << nou << std::endl << std::endl;

  out << "terreny" << std::endl;
  out << "   ";
  for (int j = 0; j < MAX; ++j) out << (j / 10);
  out << std::endl;

  out << "   ";
  for (int j = 0; j < MAX; ++j) out << (j % 10);
  out << std::endl;
  out << std::endl;

  for (int i = 0; i < MAX; ++i) {
    out << (i / 10) << (i % 10) << " "; // Adding col numbers.
    for (int j = 0; j < MAX; ++j)
      out << code[terreny[i][j]];
    out << std::endl;
  }

  out << std::endl;
  out << "posts" << std::endl;
  out << "equip"  << "\t"
      << "x"      << "\t"
      << "y"      << "\t"
      << "valor"      << "\t"
      << std::endl;
  for (const auto& p : post)
    out << p.equip 
	<< "\t" << p.pos.x 
	<< "\t" << p.pos.y 
	<< "\t" << p.valor
	<< std::endl;
  out << std::endl;

  out << "score" << std::endl;
  for (int e = 1; e <= 4; ++e)
    out << puntuacio(e) << "\t";
  out << std::endl << std::endl;

  out << "status" << std::endl;
  for (int e = 1; e <= 4; ++e)
    out << status(e) << "\t";
  out << std::endl << std::endl;

  out << "agents" << std::endl;
  out << "id"     << "\t"
      << "tipus"  << "\t"
      << "equip"  << "\t"
      << "x"      << "\t"
      << "y"      << "\t"
      << "vida"   << "\t"
      << "ori"    << "\t"
      << "napalm" << "\t"
      << "paraca" << "\t"
      << std::endl;
  for (const auto& p: dada) {
    int id     = p.second.id;
    int tipus  = p.second.tipus;
    int equip  = p.second.equip;
    int x      = p.second.pos.x;
    int y      = p.second.pos.y;
    int vida   = p.second.vida;
    int ori    = p.second.orientacio;
    int napalm = p.second.napalm;
    int psize  = p.second.paraca.size();
    out << id 
	<< "\t" << tipus 
	<< "\t" << equip 
	<< "\t" << x 
	<< "\t" << y 
	<< "\t" << vida 
	<< "\t" << ori 
	<< "\t" << napalm 
	<< "\t" << psize;
    for (int x : p.second.paraca)
      out << "\t" << x;
 
	
    out << "\t" << std::endl;
  }
  out << "-1" << std::endl;
  out << std::endl;

  out << "foc" << std::endl;
  out << "torns" << "\t" << "x" << "\t" << "y" << std::endl;
  for (int x = 0; x < MAX; ++x)
    for (int y = 0; y < MAX; ++y)
      if (foc[x][y] != 0)
	out << foc[x][y] << "\t" << x << "\t" << y << std::endl;
  out << "-1" << std::endl << std::endl;
}


Board Board::next(const std::vector<Action>& asked, std::vector<Action>& done) const {
  assert(ok());

  Board b = *this;

  VVP ordres3(5);
  for (int e = 1; e <= 4; ++e)
    ordres3[e] = asked[e].ordres3;
  b.paracaigudistes_baixen(ordres3, done);

  std::vector<P> ordres2;
  for (int e = 1; e <= 4; ++e)
    for (auto& p : asked[e].ordres2)
      ordres2.push_back(p);
  b.fes_torn_helis(ordres2, done);

  std::vector<P2> ordres1;
  for (int e = 1; e <= 4; ++e)
    for (auto& p : asked[e].ordres1)
      ordres1.push_back(p);
  b.fes_torn_soldats(ordres1, done);

  b.propaga_foc();
  ++b.torn;

  assert(b.ok());
  return b;
}


bool Board::ok(void) const {

  if (!(0 <= torn && torn <= TORNS_JOC)) {
    error("torn té un valor invàlid");
    return false;
  }

  for (int x = 0; x < MAX; ++x)
    for (int y = 0; y < MAX; ++y) {
      if (foc[x][y] < 0) {
	error("foc no pot tenir valors negatius");
	return false;
      }
      if (foc[x][y] > 0 && terreny[x][y] == MUNTANYA) {
	error("hi ha foc en muntanya");
	return false;
      }
      if (terreny[x][y] != BOSC && foc[x][y] > CREMA_ALTRE) {
	error("foc > CREMA_ALTRE en terreny no boscós");
	return false;
      }
      if (terreny[x][y] == BOSC && foc[x][y] > CREMA_BOSC) {
	error("foc > CREMA_BOSC en terreny boscós");
	return false;
      }
    }

  for (int k = 0; k < MAX; ++k)
    if (terreny[  k  ][  0  ] != MUNTANYA ||
	terreny[  k  ][MAX-1] != MUNTANYA ||
	terreny[  0  ][  k  ] != MUNTANYA ||
	terreny[MAX-1][  k  ] != MUNTANYA) {
      error("el tauler ha d'estar rodejat de muntanyes");
      return false;
    }

  int cnt_ids = 0;
  int cnt_cel = 0;
  for (int x = 0; x < MAX; ++x)
    for (int y = 0; y < MAX; ++y) {
      int id_soldat = iden[SOLDAT][x][y];
      if (id_soldat != 0) {
	auto it = dada.find(id_soldat);
	if (it == dada.end()) {
	  error("identificador de soldat no registrat");
	  return false;
	}
	Info info = it->second;
	if (info.pos != Posicio(x, y)) {
	  error("problema amb la posició de soldat registrada");
	  return false;
	}
	if (terreny[x][y] == MUNTANYA || terreny[x][y] == AIGUA) {
	  PRINT(x);
	  PRINT(y);
	  PRINT(terreny[x][y]);
	  error("els soldats no poden anar per muntanya o aigua");
	  return false;
	}
	++cnt_ids;
      }
      int id_heli = iden[HELI][x][y];
      if (id_heli != 0) {
	auto it = dada.find(id_heli);
	if (it == dada.end()) {
	  error("identificador d'helicòpter no registrat");
	  return false;
	}
	Info info = it->second;
	if (info.pos != Posicio(x, y)) {
	  error("problema amb la posició d'helicòpter registrada");
	  return false;
	}
	for (int dx = -2; dx <= 2; ++dx)
	  for (int dy = -2; dy <= 2; ++dy) {
	    int xx = x + dx;
	    int yy = y + dy;
	    if (! valid(xx, yy)) {
	      error("helicòpter fora de tauler");
	      return false;
	    }
	    if (terreny[xx][yy] == MUNTANYA) {
	      error("els helicòpters no poden anar per muntanya");
	      return false;
	    }
	    if (! cel[xx][yy]) {
	      error("cel no consta com a ocupat");
	      return false;
	    }
	    ++cnt_cel;
	  }
	++cnt_ids;
      }
    }
  if (cnt_ids != int(dada.size())) {
    error("mides no quadren");
    return false;
  }

  int cnt_cel2 = 0;
  for (int x = 0; x < MAX; ++x)
    for (int y = 0; y < MAX; ++y)
      if (cel[x][y])
	++cnt_cel2;
  if (cnt_cel != cnt_cel2) {
    error("no quadra el nombre de caselles de cel ocupades");
    return false;
  }

  int cnt_post = 0;
  for (int x = 0; x < MAX; ++x)
    for (int y = 0; y < MAX; ++y) {
      if (quipost[x][y] != 0)
	++cnt_post;
      if (quipost[x][y] < -1 || quipost[x][y] > 4) {
	error("valor erroni a quipost");
	return false;
      }
    }
  if (cnt_post != NUM_POSTS) {
    error("no quadra el nombre de posts");
    return false;
  }

  cnt_post = 0;
  for (int x = 0; x < MAX; ++x)
    for (int y = 0; y < MAX; ++y) {
      if (valorpost[x][y] != 0) {
	++cnt_post;
	if (valorpost[x][y] != VALOR_ALT && valorpost[x][y] != VALOR_BAIX) {
	  error("valor erroni a valorpost");
	  return false;
	}
      }
    }
  if (cnt_post != NUM_POSTS) {
    error("no quadra el nombre de posts");
    return false;
  }
  if (int(post.size()) != NUM_POSTS) {
    error("nombre de posts no quadra");
    return false;
  }
  for (int k = 0; k < int(post.size()); ++k) {
    int equip = post[k].equip;
    int x = post[k].pos.x;
    int y = post[k].pos.y;
    int valor = post[k].valor;
    if (quipost[x][y] != equip) {
      error("les dades de post no quadren");
      return false;
    }
    if (valorpost[x][y] != valor) {
      error("les dades de post no quadren");
      return false;
    }
  }

  for (const auto& p : dada) {
    int    id = p.first;
    Info info = p.second;
    if (id != info.id) {
      error("identificadors no quadren");
      return false;
    }
    if (info.tipus != SOLDAT && info.tipus != HELI) {
      error("tipus incorrecte");
      return false;
    }
    if (info.tipus == HELI && info.vida != -1) {
      error("els helicòpters mai moren");
      return false;
    }
    if (info.tipus == SOLDAT && (info.vida < 0 || info.vida > VIDA)) {
      error("valor de vida incorrecte");
      return false;
    }
    if (info.tipus == HELI && (info.orientacio < 0 || info.orientacio > 3)) {
      error("valor de orientació incorrecte");
      return false;
    }
    if (info.tipus == SOLDAT && info.orientacio != -1) {
      error("els soldats no tenen orientació");
      return false;
    }
    if (info.tipus == HELI && (info.napalm < 0 || info.napalm > TORNS_NAPALM)) {
      error("valor de napalm incorrecte");
      return false;
    }
    if (info.tipus == SOLDAT && info.napalm != -1) {
      error("els soldats no tenen napalm");
      return false;
    }
    if (info.tipus == SOLDAT && info.paraca != VE()) {
      error("els soldats no tenen paracaigudistes");
      return false;
    }
    for (int r : info.paraca)
      if (r < 0 || r > TORNS_BAIXAR) {
	error("paracaigudistes amb nombre de torns per baixar erroni");
	return false;
      }
  }

  int n_agents = 0;
  for (int e = 1; e <= 4; ++e)
    n_agents += soldat[e].size() + heli[e].size();

  if (n_agents != int(dada.size())) {
    PRINT(n_agents);
    PRINT(dada.size());
    error("dades no quadren");
    return false;
  }

  for (int e = 1; e <= 4; ++e) {
    for (int id : soldat[e]) {
      auto it = dada.find(id);
      if (it == dada.end()) {
	error("soldat no registrat");
	return false;
      }
      int equip = it->second.equip;
      if (e != equip) {
	error("equips de soldats no quadren");
	return false;
      }
    }
    for (int id : heli[e]) {
      auto it = dada.find(id);
      if (it == dada.end()) {
	error("helicòpter no registrat");
	return false;
      }
      int equip = it->second.equip;
      if (e != equip) {
	error("equips d'helicòpters no quadren");
	return false;
      }
    }
  }

  int n_soldats_o_paracas = 0;
  for (int e = 1; e <= 4; ++e) {
    n_soldats_o_paracas += soldat[e].size();
    for (auto id : heli[e]) {
      auto it = dada.find(id);
      n_soldats_o_paracas += it->second.paraca.size();
    }
  }
  if (n_soldats_o_paracas != 4 * NUM_SOLDATS) {
    error("el nombre de soldats + paracaigudistes no quadra");
    return false;
  }

  return true;
}
