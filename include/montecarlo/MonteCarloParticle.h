#ifndef MONTECARLOPARTICLE_H
#define MONTECARLOPARTICLE_H

/**
 * Represents one particle.
 */
class MonteCarloParticle
{
  /**
   * Constructor
   *
   * @param id The unique ID for this particle.
   */
//  MonteCarloParticle();


  /**
   * Get the current position.
   */
  const Point & position() { return _position; }

  /**
   * Set the current position.
   */
  void setPosition(const Point & p) { _position = p; }

  /**
   * Grab the next random number for this particle.
   *
   * Returns a number between 0 and 1.
   */
  double nextRand() { return rand_r(&_seed) / RAND_MAX; }

private:
  Point _position;

  /**
   * Initial seed and state data for the random stream.
   */
  unsigned int _seed;
};


#endif
