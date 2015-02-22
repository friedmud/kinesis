#ifndef MONTECARLOBOUNDARY_H
#define MONTECARLOBOUNDARY_H

// MOOSE
#include "Moose.h"
#include "MooseTypes.h"
#include "LineSegment.h"

/**
 * Represents a boundary that paths can intersect with
 */
class MonteCarloBoundary
{
public:

  /**
   * Initialize the boundary.
   *
   * @param connected_subdomains The subdomains this boundary is connected to
   */
  MonteCarloBoundary(const std::vector<SubdomainID> & connected_subdomains) : _connected_subdomains(connected_subdomains) {}

  virtual ~MonteCarloBoundary() {}


  /**
   * See if the path intersects this boundary.
   *
   * @param path The current path of the particle
   * @param intersection_point If the path does intersect this will get filled with the intersection point
   * @return True if the path intersecrts this boundary, false otherwise
   */
  virtual bool intersect(const LineSegment & path, Point & intersection_point) = 0;

  /**
   * Get the subdomains that are connected to this boundary
   */
  const std::vector<SubdomainID> & connectedSubdomains() { return _connected_subdomains; }

  /**
   * Get the _other_ subdomain.  Pass in one subdomain that's connected to the boundary and you'll get back the other one.
   */
  SubdomainID otherSubdomain(SubdomainID subdomain);


protected:
  /// The subdomains connected to this boundary
  std::vector<SubdomainID> _connected_subdomains;
};


#endif
