#include "MonteCarloParticle.h"



MonteCarloParticle::MonteCarloParticle(unsigned int id)
    : _id(id),
      _seed(id), // Start the seed with the particle ID
      _current_subdomain(Moose::INVALID_BLOCK_ID),
      _intersected_boundary(false)
{
  // Burn a few random numbers to get to the good ones :-)
  for (unsigned int i=0; i<4; i++)
    rand_r(&_seed);
}
