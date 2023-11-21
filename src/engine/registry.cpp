#include "registry.hpp"

typedef std::map<std::string, Registry::Factory> dict;

std::unique_ptr<dict> reg;

int Registry::Register(const char *name, Factory factory) {
    if (reg == nullptr) {
        reg = std::make_unique<dict>();
    }
    (*reg)[name] = factory;
    return 999;
}

Player *Registry::newPlayer(std::string name) {
    dict::iterator it = reg->find(name);
    if (it == reg->end()) {
        error("player " + name + " ! registered.");
        std::exit(EXIT_FAILURE);
    }
    Factory f = it->second;
    return f();
}

void Registry::printPlayers(std::ostream &os) {
    for (dict::iterator it = reg->begin(); it != reg->end(); ++it) {
        os << it->first << std::endl;
    }
}
