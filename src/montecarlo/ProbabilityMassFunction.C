#include "ProbabilityMassFunction.h"

#include "MooseError.h"

unsigned int
ProbabilityMassFunction::getEvent(Real random_value)
{
  mooseAssert(0.0 <= random_value && random_value <= 1.0, "Invalid random variable value: " << random_value);

  // linear search (for now)
  for (unsigned int i=0; i<_num_probabilities; i++)
  {
    if (random_value <= _probabilities[i])
      return i;
  }

  mooseError("Shouldn't be here!");
}
