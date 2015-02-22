#ifndef PROBABILITYMASSFUNCTION_H
#define PROBABILITYMASSFUNCTION_H

// MOOSE
#include "Moose.h"
#include "MooseTypes.h"

/**
 * Represents a discrete probability density function
 */
class ProbabilityMassFunction
{
public:

  /**
   * Pass in the raw probability for each event.  They will be normalized automatically.
   *
   * The order you pass them in will assign their IDs (0 through n-1)
   */
  ProbabilityMassFunction(const std::vector<Real> & raw_probabilities)
      :_num_probabilities(raw_probabilities.size()),
       _probabilities(_num_probabilities)
    {
      // Find the total
      Real total = 0;
      for (unsigned int i=0; i<_num_probabilities; i++)
        total += raw_probabilities[i];

      // Fill in the normalized probabilities
      Real running_probability = 0;

      for (unsigned int i=0; i<_num_probabilities; i++)
      {
        running_probability += raw_probabilities[i] / total;

        _probabilities[i] = running_probability;
      }
    }

  /**
   * Get the ID for the event that corresponds to the random value.
   *
   * @param random_value A random variable between 0 and 1
   * @return The ID (0 through n-1) for the event.
   */
  unsigned int getEvent(Real random_value);

protected:
  /// Number of probabilities (for speed)
  unsigned int _num_probabilities;

  /// The normalized probabilities
  std::vector<Real> _probabilities;
};


#endif
