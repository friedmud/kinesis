#ifndef TALLYGRID_H
#define TALLYGRID_H

// MOOSE
#include "Moose.h"
#include "MooseTypes.h"

/**
 * Grid to Tally on.
 */
class TallyGrid
{
public:
  TallyGrid(Real domain_beginning, Real domain_end, unsigned int bins, Real total_starting_weight);

  /**
   * Tally a collision at point p
   *
   *@param p The point where the collision occurred.
   *@param weight The particle weight
   *@param sigma_t The total macroscopic cross section
   */
  void tallyCollision(const Point & p, Real weight, Real sigma_t);

  /**
   * Do final operations on the tallys to make them right
   */
  void finalize();

protected:
  /// Beginning of the domain
  Real _domain_beginning;

  /// End of the domain
  Real _domain_end;

  /// Number of tally bins
  Real _bins;

  /// Total domain size
  Real _domain_size;

  /// Size of each interval
  Real _interval_size;

  /// Total starting weight of all particles
  Real _total_starting_weight;

  /// The running collision tally
  std::vector<Real> _collision_tally;
};


#endif
