#pragma once
#include <ncs/sim/Neuron.h>

namespace ncs {

namespace sim {

struct Synapse {
  unsigned int seed;
  void* instantiator;
  Neuron* presynaptic_neuron;
  Neuron* postsynaptic_neuron;
  struct {
    unsigned int plugin;
  } id;
  struct {
    unsigned int plugin;
    unsigned int device;
    unsigned int machine;
  } location;
};

} // namespace sim

} // namespace ncs
