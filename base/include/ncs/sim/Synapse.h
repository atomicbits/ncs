#pragma once
#include <ncs/sim/Neuron.h>

namespace ncs {

namespace sim {

struct Synapse {
  unsigned int seed;
  void* instantiator;
  Neuron* presynaptic_neuron;
  Neuron* postsynaptic_neuron;
  unsigned int delay;
  struct {
    unsigned int plugin;
    unsigned int device;
  } id;
  Location location;
};

} // namespace sim

} // namespace ncs

