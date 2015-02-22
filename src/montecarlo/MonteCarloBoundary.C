#include "MonteCarloBoundary.h"


// Moose
#include "MooseError.h"

SubdomainID
MonteCarloBoundary::otherSubdomain(SubdomainID subdomain)
{
  mooseAssert(_connected_subdomains.size(), "Boundary not initialized properly.. it doesn't have any connected subdomains");

  if (subdomain == Moose::INVALID_BLOCK_ID)
  {
    mooseAssert(_connected_subdomains.size() == 1, "Something is weird here");  // There should only be one boundary connected
    return _connected_subdomains[0];
  }

  // This means we're on an external boundary
  if (_connected_subdomains.size() == 1)
    return Moose::INVALID_BLOCK_ID;

  // Figure out which other subdomain we're connected to here:
  for (unsigned int i=0; i<_connected_subdomains.size(); i++)
    if (_connected_subdomains[i] != subdomain)
      return _connected_subdomains[i];

  mooseError("Shouldn't get here!");
}
