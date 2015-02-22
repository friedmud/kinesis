#include "TallyGrid.h"

#include "MooseError.h"


TallyGrid::TallyGrid(Real domain_beginning, Real domain_end, unsigned int bins, Real total_starting_weight)
    :_domain_beginning(domain_beginning),
     _domain_end(domain_end),
     _bins(bins),
     _domain_size(_domain_end - _domain_beginning),
     _interval_size(_domain_size/bins),
     _total_starting_weight(total_starting_weight),
     _num_histories(0),
     _collision_tally(_bins),
     _total_collision_count(_bins),
     _total_square_collision_count(_bins),
     _mean(_bins),
     _variance(_bins)
{
}


void
TallyGrid::tallyCollision(const Point & p, Real weight, Real sigma_t)
{
  unsigned int index = binIndex(p);

  _collision_tally[index] += weight / sigma_t;

  _bin_to_hits[index] += 1;
}


void
TallyGrid::beginHistory()
{
  _num_histories++;

  // Clear the particle collision count
  _bin_to_hits.clear();
}


void
TallyGrid::endHistory()
{
  std::map<unsigned int, unsigned int>::iterator i = _bin_to_hits.begin();
  const std::map<unsigned int, unsigned int>::iterator end = _bin_to_hits.end();

  for(; i != end; ++i)
  {
    unsigned int bin = i->first;
    unsigned int hits = i->second;

    _total_collision_count[bin] += hits;
    _total_square_collision_count[bin] += hits*hits;
  }
}


void
TallyGrid::finalize()
{
  Real collision_divisor = 1/(_interval_size * _total_starting_weight);

  for (unsigned int i=0; i<_bins; i++)
    _collision_tally[i] *= collision_divisor;

  for (unsigned int i=0; i<_bins; i++)
    _mean[i] = _total_collision_count[i] / _num_histories;

  for (unsigned int i=0; i<_bins; i++)
    _variance[i] = std::sqrt( (1/(_num_histories - 1)) * ( (_total_square_collision_count[i]/_num_histories) - (_mean[i]*_mean[i]) ) );
}

unsigned int
TallyGrid::binIndex(const Point & p)
{
  Real x_coord = p(0);

  unsigned int index = (int)(x_coord / _interval_size);

  // If we barely hit then end then make sure we tally to the last bin
  if (index == _bins)
    index--;

  return index;
}
