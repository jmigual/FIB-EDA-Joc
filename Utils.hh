#ifndef Utils_hh
#define Utils_hh

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <limits>
#include <cassert>
#include <cstdlib>
#include <cerrno>
#include <time.h>
#include <algorithm>
#include <sys/time.h>
//#include <sys/resource.h>

using namespace std;


///////////// conversió a / des de strings ////////////////

// Transforma un string en un int.
inline int s2i(const string& s) {
  istringstream iss(s);
  int i;
  iss >> i;
  return i;
}

// Transforma un int en un string.
inline string i2s(int i) {
  ostringstream oss;
  oss << i;
  return oss.str();
}

// Transforma un double en un string.
inline string d2s(double p) {
  ostringstream ss;
  ss.setf(ios::fixed);
  ss.precision(6);
  ss << p;
  return ss.str();
}


////////////////// debugging //////////////////////////

inline void error(string s) { cerr << "error: " << s << endl; }

// PRINT(x) escriu per pantalla "x: <valor de x>"

#ifndef PRINT
#define PRINT_AUX(ARG,SEP)  {string tmpStr(SEP # ARG # SEP);	\
    cerr << boolalpha						\
	 << tmpStr.substr(1, tmpStr.size()-5)			\
	 << ": " << ARG << endl;}
#define PRINT(ARG)          PRINT_AUX(ARG,"\"")
#endif


////////////////// nombres aleatoris //////////////////////////

class atzar {
private:

  void seguent();

public:
  int llavor;

  atzar(): llavor(0) {}
  
  // inicialitza la llavor amb el valor donat
  atzar(int llavor);

  // retorna un real aleatori de [0, 1)
  double uniforme();

  // retorna un enter aleatori de [esquerra, dreta]
  int uniforme(int esquerra, int dreta);

  // retorna un real aleatori de [esquerra, dreta] amb dig digits
  double uniforme(int esquerra, int dreta, int dig);

  // retorna true amb probabilitat p
  bool probabilitat(double p);

  // retorna una permutació aleatòria de 0 .. n - 1
  vector<int> permutacio(int n);

  int tria(int n) { return (int)(uniforme()*(double)n); }
};

#endif
