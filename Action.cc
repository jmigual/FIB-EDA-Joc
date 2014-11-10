#include "Action.hh"
#include "Board.hh"

using namespace std;


Action::Action (istream& is) {

  ordres1.clear();
  ordres2.clear();
  ordres3.clear();

  int id;
  while (is >> id and id != -1) {
    int x, y;
    is >> x >> y;
    ordres1[id] = Posicio(x, y);
  }

  while (is >> id and id != -1) {
    int d;
    is >> d;
    ordres2[id] = d;
  }

  int x, y;
  while (is >> x and x != -1) {
    is >> y;
    ordres3.push_back(Posicio(x, y));
  }
}

void Action::print (ostream& os) const {
  for (const auto& p : ordres1)
    os << p.first << " " << p.second.x << " " << p.second.y << endl;
  os << "-1" << endl;

  for (const auto& p : ordres2)
    os << p.first << " " << p.second << endl;
  os << "-1" << endl;

  for (const auto& p : ordres3)
    os << p.x << " " << p.y << endl;
  os << "-1" << endl;
}
