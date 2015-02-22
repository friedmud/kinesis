#include "MonteCarloUserObject.h"

// Kinesis
#include "MonteCarloParticle.h"
#include "PlanarMonteCarloBoundary.h"

// Moose
#include "Executioner.h"
#include "MooseError.h"

template<>
InputParameters validParams<MonteCarloUserObject>()
{
  InputParameters params = validParams<GeneralUserObject>();

  params.addRequiredParam<unsigned int>("num_particles", "The total number of particles to track");
  params.addRequiredParam<std::vector<Real> >("boundaries", "Edges of slabs: beginning of slab1, beginning of slab2, end of slab2");
  params.addRequiredParam<std::vector<Real> >("sigma_t", "Total cross section of each slab: slab1, slab2");
  params.addRequiredParam<std::vector<Real> >("sigma_a", "Absorption cross section of each slab: slab1, slab2");

  return params;
}

MonteCarloUserObject::MonteCarloUserObject(const std::string & name, InputParameters parameters) :
    GeneralUserObject(name, parameters),
    _num_particles(getParam<unsigned int>("num_particles")),
    _boundaries(getParam<std::vector<Real> >("boundaries")),
    _num_boundaries(_boundaries.size()),
    _monte_carlo_boundaries(_num_boundaries),
    _subdomain_boundaries(_num_boundaries - 1),
    _sigma_t(getParam<std::vector<Real> >("sigma_t")),
    _sigma_a(getParam<std::vector<Real> >("sigma_a"))
{
  std::cout<<"Here!"<<std::endl;

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

  std::cout<<"Here Again!"<<std::endl;
}

MonteCarloUserObject::~MonteCarloUserObject()
{
  for (unsigned int i=0; i<_monte_carlo_boundaries.size(); i++)
    delete _monte_carlo_boundaries[i];
}

void
MonteCarloUserObject::execute()
{
  // Make this out here and just reuse it a bunch so that it doesn't need to get created and destroyed
  Point new_position;
  Point new_direction;
  Point boundary_intersection;

  for (unsigned int i=0; i<_num_particles; i++)
  {
    MonteCarloParticle particle(i);

    particle.setPosition(Point(0,0,0));

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
        /*
        new_direction(0) = mu;
        new_direction(1) = sqrt_one_minus_mu2 * std::cos(phi);
        new_direction(2) = sqrt_one_minus_mu2 * std::sin(phi);
        */

        new_direction(0) = 1.0/std::sqrt(2);
        new_direction(1) = 1.0/std::sqrt(2);
        new_direction(2) = 0;
      }
      else
      {
        std::cout<<"Using old direction!"<<std::endl;
        new_direction = particle.direction();
      }

      distance = std::sqrt(2);

      std::cout<<"New direction: "<<new_direction<<std::endl;

      // Start building up the new position
      new_position = particle.position();

      // Compute the new position
      new_position.add_scaled(new_direction, distance);

      std::cout<<"New position: "<<new_position<<std::endl;

      SubdomainID current_subdomain = particle.currentSubdomain();
      SubdomainID new_subdomain = subdomainContainingPoint(new_position);

      std::cout<<"Subdomains: "<<current_subdomain<<" "<<new_subdomain<<std::endl;

      // Did we cross a boundary?
      if (new_subdomain != current_subdomain)
      {
        std::cout<<"Crossed boundary!"<<std::endl;

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
            std::cout<<"Setting intersection point to: "<<boundary_intersection<<std::endl;

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
        {
          std::cout<<"Weird!"<<std::endl;
          particle.setPosition(new_position);
        }
      }
      else
      {
        particle.setPosition(new_position);

        particle.setIntersectedBoundary(false);
      }

      // Leakage
      if (particle.currentSubdomain() == Moose::INVALID_BLOCK_ID)
      {
        std::cout<<"Leaked!"<<std::endl;
        break;
      }
    }
  }
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
