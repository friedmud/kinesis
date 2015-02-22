#ifndef MONTECARLOUSEROBJECT_H
#define MONTECARLOUSEROBJECT_H

// Kinesis
#include "TallyGrid.h"

// Moose
#include "GeneralUserObject.h"

//Forward Declarations
class MonteCarloUserObject;
class MonteCarloParticle;
class MonteCarloBoundary;
class ProbabilityMassFunction;

template<>
InputParameters validParams<MonteCarloUserObject>();

/**
  * Monte Carlo particle simulation
  */
class MonteCarloUserObject : public GeneralUserObject
{
public:
  MonteCarloUserObject(const std::string & name, InputParameters parameters);
  virtual ~MonteCarloUserObject();

  virtual void execute();
  virtual void initialize() {};
  virtual void finalize() {};

protected:

  /// Total number of particles
  unsigned int _num_particles;

  /// Boundaries of the slabs
  std::vector<Real> _boundaries;

  /// Number of entries in _boundaries (for speed)
  unsigned int _num_boundaries;

  /// Number of subdomains
  unsigned int _num_subdomains;

  /// Planes for each boundary
  std::vector<MonteCarloBoundary *> _monte_carlo_boundaries;

  /// Boundaries connected to each subdomain
  std::vector<std::vector<MonteCarloBoundary *> > _subdomain_boundaries;

  /// Total cross section
  std::vector<Real> _sigma_t;

  /// Absorption cross section
  std::vector<Real> _sigma_a;

  /// Reaction probability mass functions for each subdomain
  std::vector<ProbabilityMassFunction *> _subdomain_reaction_pmfs;

  /// The subdomain that contains the source
  unsigned int _source_subdomain;

  /// The size of the source subdomain
  Real _source_subdomain_size;

  /// The beginning of the source subdomain
  Real _source_subdomain_beginning;

  /// The number of tally bins
  unsigned int _bins;

  /// The tallying datastructure
  TallyGrid _tally_grid;

  /**
   * Get the distance the particle is going to travel.
   */
  Real computeDistance(MonteCarloParticle & particle);

  /**
   * Get the mu
   *
   * @return Number between -1..1
   */
  Real computeMu(MonteCarloParticle & particle);

  /**
   * Get the phi (azimuthal angle)
   *
   * @return Number between 0 and 2*pi
   */
  Real computePhi(MonteCarloParticle & particle);

  /**
   * Get the subdomain the Point falls in.
   *
   * @param p The Point
   * @return The subdomain.  Returns invalid_subdomain_id if outside of the domain.
   */
  SubdomainID subdomainContainingPoint(const Point & p);
};

#endif //MONTECARLOUSEROBJECT_H
