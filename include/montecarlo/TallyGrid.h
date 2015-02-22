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
   * Called before each particle history begins to reset datastructures
   */
  void beginHistory();

  /**
   * Do final operations for this history - summing into global tally arrays
   */
  void endHistory();

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

  /// Total number of histories (as recorded by the number of calls to beginHistory()
  unsigned int _num_histories;

  /// The total collision rate tally for all histories
  std::vector<Real> _collision_tally;

  /// Number of collisions made by all particles
  std::vector<unsigned int> _total_collision_count;

  /// Sum of the square of each particle's collisions
  std::vector<unsigned int> _total_square_collision_count;

  /// Tallies for a single particle history
  std::map<unsigned int, unsigned int> _bin_to_hits;

  /// Mean
  std::vector<Real> _mean;

  /// Variance
  std::vector<Real> _variance;

private:

  /**
   * Get the bin index for a spatial position.
   */
  unsigned int binIndex(const Point & p);
};


#endif
