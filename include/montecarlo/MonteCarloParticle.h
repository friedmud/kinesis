#ifndef MONTECARLOPARTICLE_H
#define MONTECARLOPARTICLE_H

// MOOSE
#include "Moose.h"
#include "MooseTypes.h"

// libMesh
#include "libmesh/point.h"

/**
 * Represents one particle.
 */
class MonteCarloParticle
{
public:
  /**
   * Constructor
   *
   * @param id The unique ID for this particle.
   */
  MonteCarloParticle(unsigned long int id);

  /**
   * Get the ID for the particle
   */
  unsigned long int id();

  /**
   * Get the current position.
   */
  const Point & position() { return _position; }

  /**
   * Set the current position.
   */
  void setPosition(const Point & p) { _position = p; }

  /**
   * Move the particle by the passed in vector.
   *
   * @param direction This is added to the current position to move the particle.
   */
  void move(const Point & direction, Real distance) { _position.add_scaled(direction, distance); }

  /**
   * Set the current subdomain the particle is in.
   */
  void setCurrentSubdomain(SubdomainID subdomain) { _current_subdomain = subdomain; }

  /**
   * Get the current subdomain.
   */
  SubdomainID currentSubdomain() { return _current_subdomain; }

  /**
   * Grab the next random number for this particle.
   *
   * Returns a number between 0 and 1.
   */
  Real nextRand() { return (Real)rand_r(&_seed) / RAND_MAX; }

  /**
   * Set the direction the particle is traveling in
   *
   * @param direction A unit vector in the direction the particle is traveling in
   */
  void setDirection(const Point & direction) { _direction = direction; }

  /**
   * Get the direction this particle is moving in.
   *
   * @return a unit vector representing the direction the particle is moving.
   */
  const Point & direction() { return _direction; }

  /**
   * Set whether or not the particle just hit a boundary.
   *
   * @param intersected Whether or not the particle just intersected a boundary.  If it did then the particle needs to keep traveling in the same direction in was
   */
  void setIntersectedBoundary(bool intersected) { _intersected_boundary = intersected; }

  /**
   * See whether or not the particle intersected a boundary (ie, it needs to keep traveling in the same direction
   */
  bool intersectedBoundary() { return _intersected_boundary; }

private:

  /// The particle ID
  unsigned long int _id;

  /// The current position of the particle
  Point _position;

  /// Initial seed and state data for the random stream.
  unsigned int _seed;

  /// The subdomain the particle is currently in.
  SubdomainID _current_subdomain;

  /// True if the particle just intersected a boundary and needs to keep traveling in that direction
  bool _intersected_boundary;

  /// Unit vector specifying the direction the particle is currently traveling
  Point _direction;
};


#endif
