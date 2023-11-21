#ifndef Utils_hh
#define Utils_hh

// IWYU pragma: begin_exports
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
// #include <sys/time.h>
//#include <sys/resource.h>
// IWYU pragma: end_exports



///////////// conversió a / des de strings ////////////////

// Transforma un string en un int.
inline int s2i(const std::string& s) {
  return std::stoi(s);
}

// Transforma un int en un string.
inline std::string i2s(int i) {
  return std::to_string(i);
}

// Transforma un double en un string.
inline std::string d2s(double p) {
  return std::to_string(p);
}


////////////////// debugging //////////////////////////

inline void error(std::string && s) { std::cerr << "error: " << s << std::endl; }

// PRINT(x) Writes on screen "x: <value of x>"

#ifndef PRINT
#define PRINT_AUX(ARG,SEP)  {std::string tmpStr(SEP # ARG # SEP);	\
    std::cerr << std::boolalpha						\
	 << tmpStr.substr(1, tmpStr.size()-5)			\
	 << ": " << ARG << std::endl;}
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
  std::vector<int> permutacio(int n);

  int tria(int n) { return (int)(uniforme()*(double)n); }
};

#endif
