#include "TallyGrid.h"

#include "MooseError.h"


TallyGrid::TallyGrid(Real domain_beginning, Real domain_end, unsigned int bins, Real total_starting_weight)
    :_domain_beginning(domain_beginning),
     _domain_end(domain_end),
     _bins(bins),
     _domain_size(_domain_end - _domain_beginning),
     _interval_size(_domain_size/bins),
     _total_starting_weight(total_starting_weight),
     _collision_tally(_bins)
{
}


void
TallyGrid::tallyCollision(const Point & p, Real weight, Real sigma_t)
{
  Real x_coord = p(0);

  unsigned int index = (int)(x_coord / _interval_size);

  // If we barely hit then end then make sure we tally to the last bin
  if (index == _bins)
    index--;

  _collision_tally[index] += weight / sigma_t;
}

void
TallyGrid::finalize()
{
  Real collision_divisor = 1/(_interval_size * _total_starting_weight);

  for (unsigned int i=0; i<_bins; i++)
  {
    _collision_tally[i] *= collision_divisor;
    std::cout<<_collision_tally[i]<<std::endl;
  }
}
