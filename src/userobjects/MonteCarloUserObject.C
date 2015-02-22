#include "MonteCarloUserObject.h"

// Kinesis
#include "MonteCarloParticle.h"
#include "PlanarMonteCarloBoundary.h"
#include "ProbabilityMassFunction.h"

// Moose
#include "Executioner.h"
#include "MooseError.h"

// System
#include <chrono>

template<>
InputParameters validParams<MonteCarloUserObject>()
{
  InputParameters params = validParams<GeneralUserObject>();

  params.addRequiredParam<unsigned int>("num_particles", "The total number of particles to track");
  params.addRequiredParam<std::vector<Real> >("boundaries", "Edges of slabs: beginning of slab1, beginning of slab2, end of slab2");
  params.addRequiredParam<std::vector<Real> >("sigma_t", "Total cross section of each slab: slab1, slab2");
  params.addRequiredParam<std::vector<Real> >("sigma_a", "Absorption cross section of each slab: slab1, slab2");
  params.addRequiredParam<unsigned int>("source_subdomain", "The subdomain (starting at 0) containing the source");

  return params;
}

MonteCarloUserObject::MonteCarloUserObject(const std::string & name, InputParameters parameters) :
    GeneralUserObject(name, parameters),
    _num_particles(getParam<unsigned int>("num_particles")),
    _boundaries(getParam<std::vector<Real> >("boundaries")),
    _num_boundaries(_boundaries.size()),
    _num_subdomains(_num_boundaries - 1),
    _monte_carlo_boundaries(_num_boundaries),
    _subdomain_boundaries(_num_boundaries - 1),
    _sigma_t(getParam<std::vector<Real> >("sigma_t")),
    _sigma_a(getParam<std::vector<Real> >("sigma_a")),
    _subdomain_reaction_pmfs(_num_subdomains),
    _source_subdomain(getParam<unsigned int>("source_subdomain")),
    _source_subdomain_size(0),
    _source_subdomain_beginning(0)
{
  // Build boundary objects
  Point normal(1,0,0);
  for (unsigned int i=0; i<_num_boundaries; i++)
  {
    std::vector<SubdomainID> connected_subdomains;

    // If this isn't the first boundary then we're connected to the subdomain on the left
    if (i != 0)
      connected_subdomains.push_back(i-1);

    // If this isn't the last boundary then we're connected to the subdomain on the right
    if (i != _num_boundaries - 1)
      connected_subdomains.push_back(i);

    // Create the boundary and keep track of it
    MonteCarloBoundary * boundary = new PlanarMonteCarloBoundary(connected_subdomains, _boundaries[i], normal);
    _monte_carlo_boundaries.push_back(boundary);

    // Let's also keep a reverse map subdomain->boundaries
    for (unsigned int j=0; j<connected_subdomains.size(); j++)
      _subdomain_boundaries[connected_subdomains[j]].push_back(boundary);
  }


  // Build reaction probability mass functions for each subdomain
  for (unsigned int i=0; i<_num_subdomains; i++)
  {
    // 2 because we only have two possible reactions: collision and absorption
    std::vector<Real> raw_probs(2);

    // Collision: 0
    raw_probs[0] = _sigma_t[i] - _sigma_a[i];

    // Absorption: 1
    raw_probs[1] = _sigma_a[i];

    ProbabilityMassFunction * pmf = new ProbabilityMassFunction(raw_probs);

    _subdomain_reaction_pmfs[i] = pmf;
  }

  // Cache some values for determing the starting position of particle
  _source_subdomain_size = _boundaries[_source_subdomain+1] - _boundaries[_source_subdomain];
  _source_subdomain_beginning = _boundaries[_source_subdomain];
}

MonteCarloUserObject::~MonteCarloUserObject()
{
  for (unsigned int i=0; i<_monte_carlo_boundaries.size(); i++)
    delete _monte_carlo_boundaries[i];

  for (unsigned int i=0; i<_subdomain_reaction_pmfs.size(); i++)
    delete _subdomain_reaction_pmfs[i];
}

void
MonteCarloUserObject::execute()
{
  auto t1 = std::chrono::high_resolution_clock::now();

  // Make this out here and just reuse it a bunch so that it doesn't need to get created and destroyed
  Point new_position;
  Point new_direction;
  Point boundary_intersection;

  for (unsigned int i=0; i<_num_particles; i++)
  {
    MonteCarloParticle particle(i);

    // Determine a starting position
    Real starting_x = (particle.nextRand() * _source_subdomain_size) + _source_subdomain_beginning;

    particle.setPosition(Point(starting_x, 0, 0));

    particle.setCurrentSubdomain(subdomainContainingPoint(particle.position()));

    for (unsigned int j=0; j<20; j++)
    {
      // Distance to move
      Real distance = computeDistance(particle);

      // If this particle didn't just intersect a boundary then we need to compute a new direction for it
      if (!particle.intersectedBoundary())
      {
        // Polar angle
        Real mu = computeMu(particle);

        // Grab the azimuthal angle
        Real phi = computePhi(particle);

        // Handy square root of 1-mu^2
        Real sqrt_one_minus_mu2 = std::sqrt(1.0-(mu*mu));

        // Build a unit vector in the new direction
        // From Forrest Brown's Monte Carlo notes slide 5-10 (page 176)
        new_direction(0) = mu;
        new_direction(1) = sqrt_one_minus_mu2 * std::cos(phi);
        new_direction(2) = sqrt_one_minus_mu2 * std::sin(phi);
      }
      else
        new_direction = particle.direction();

      // Start building up the new position
      new_position = particle.position();

      // Compute the new position
      new_position.add_scaled(new_direction, distance);

      SubdomainID current_subdomain = particle.currentSubdomain();
      SubdomainID new_subdomain = subdomainContainingPoint(new_position);

      // Did we cross a boundary?
      if (new_subdomain != current_subdomain)
      {
        // Create a LineSegment for the path we were trying to take
        LineSegment path(particle.position(), new_position);

        // Find which boundary it intersected and where
        std::vector<MonteCarloBoundary *> & current_subdomain_boundaries = _subdomain_boundaries[current_subdomain];

        bool intersected = false;

        for (unsigned int b=0; b<current_subdomain_boundaries.size(); b++)
        {
          MonteCarloBoundary * boundary = current_subdomain_boundaries[b];

          // Returns whether or not the path intersects the boundary and fills "boundary_intersection" with the intersection point
          intersected = boundary->intersect(path, boundary_intersection);

          if (intersected)
          {
            // Move the particle to the intersection point
            particle.setPosition(boundary_intersection);

            // Store the direction the particle is traveling
            particle.setDirection(new_direction);

            // Tell the particle that it intersected a boundary so it can keep flying in the same direction.
            particle.setIntersectedBoundary(true);

            // We need to set the current subdomain of the particle to the one it is entering.
            particle.setCurrentSubdomain(boundary->otherSubdomain(current_subdomain));

            break;
          }
        }

        // This can happen due to floating point tolerances.  Just keep the particle in
        // the current subdomain and update its position
        if (!intersected)
          particle.setPosition(new_position);

        // Leakage
        if (particle.currentSubdomain() == Moose::INVALID_BLOCK_ID)
          break;
      }
      else // Didn't cross a boundary so let's see if we had a reaction...
      {
        particle.setPosition(new_position); // Update the particle position

        particle.setIntersectedBoundary(false); // We didn't cross a boundary

        // Determine reaction
        unsigned int reaction = _subdomain_reaction_pmfs[particle.currentSubdomain()]->getEvent(particle.nextRand());

        // 0 is Collision... and we don't need to do anything

        if (reaction == 1) // Absorption is 1
          break;
      }
    }
  }

  auto t2 = std::chrono::high_resolution_clock::now();

  std::cout<<"Histories per second: "<<(Real)_num_particles / std::chrono::duration_cast<std::chrono::seconds>(t2-t1).count()<<std::endl;
}


Real MonteCarloUserObject::computeDistance(MonteCarloParticle & particle)
{
  return -log(particle.nextRand()) / _sigma_t[particle.currentSubdomain()];
}

Real MonteCarloUserObject::computeMu(MonteCarloParticle & particle)
{
  return (2.0*particle.nextRand()) - 1.0;
}

Real MonteCarloUserObject::computePhi(MonteCarloParticle & particle)
{
  return 2*libMesh::pi*particle.nextRand();
}

SubdomainID MonteCarloUserObject::subdomainContainingPoint(const Point & p)
{
  // Snag this once for speed
  Real x_coord = p(0);

  // Is it outside the domain?
  if (x_coord < _boundaries[0] || _boundaries[_num_boundaries-1] < x_coord)
    return Moose::INVALID_BLOCK_ID;

  // Which slab is it in?
  for (unsigned int i=0; i<_num_boundaries-1; i++)
  {
    if (_boundaries[i] <= x_coord && x_coord <= _boundaries[i+1])
      return i;
  }

  // It must be in the last one
  return _num_boundaries-1;
}
