/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef TALLYVECTORPOSTPROCESSOR_H
#define TALLYVECTORPOSTPROCESSOR_H

#include "GeneralVectorPostprocessor.h"
#include "MonteCarloUserObject.h"


//Forward Declarations
class TallyVectorPostprocessor;

template<>
InputParameters validParams<TallyVectorPostprocessor>();

class TallyVectorPostprocessor : public GeneralVectorPostprocessor
{
public:
  TallyVectorPostprocessor(const std::string & name, InputParameters parameters);

  virtual ~TallyVectorPostprocessor() {}

  virtual void initialize();
  virtual void execute();

protected:
  const MonteCarloUserObject & _monte_carlo_user_object;

  VectorPostprocessorValue & _bin_centroids;
  VectorPostprocessorValue & _collision_rate;
  VectorPostprocessorValue & _flux_tally;
  VectorPostprocessorValue & _mean;
  VectorPostprocessorValue & _variance;
};

#endif
