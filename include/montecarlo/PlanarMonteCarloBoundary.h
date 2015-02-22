#ifndef PLANARMONTECARLOBOUNDARY_H
#define PLANARMONTECARLOBOUNDARY_H

// Kinesis
#include "MonteCarloBoundary.h"

// MOOSE
#include "Moose.h"
#include "MooseTypes.h"

// libMesh
#include "libmesh/plane.h"

/**
 * Represents an infinite planar boundary
 */
class PlanarMonteCarloBoundary : public MonteCarloBoundary, public Plane
{
public:

  /**
   * Initialize using a point and a normal
   *
   * @param connected_subdomains  The subdomains this boundary is connected to
   * @param p The point
   * @param n The normal
   */
  PlanarMonteCarloBoundary(const std::vector<SubdomainID> & connected_subdomains,
                           const Point & p,
                           const Point & n)
      :MonteCarloBoundary(connected_subdomains),
       Plane(p,n)
    {}

  virtual bool intersect(const LineSegment & path, Point & intersection_point) { return path.intersect(*this, intersection_point); }
};


#endif
