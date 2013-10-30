#include <iostream>

#include <ncs/spec/Generator.h>

namespace ncs {

namespace spec {

std::string Generator::generateString(RNG* rng) {
  std::cerr << name() << " cannot generate strings" << std::endl;
  std::terminate();
}

long Generator::generateInt(RNG* rng) {
  std::cerr << name() << " cannot generate integers" << std::endl;
  std::terminate();
}

double Generator::generateDouble(RNG* rng) {
  std::cerr << name() << " cannot generate doubles" << std::endl;
  std::terminate();
}

std::vector<Generator*> Generator::generateList(RNG* rng) {
  std::vector<Generator*> self;
  self.push_back(this);
  return self;
}

ModelParameters* Generator::generateParameters(RNG* rng) {
  std::cerr << name() << " cannot generate Parameters." << std::endl;
  std::terminate();
}

const std::string& Generator::name() const {
  static std::string s = "Unknown";
  return s;
}

} // namespace spec

} // namespace ncs

