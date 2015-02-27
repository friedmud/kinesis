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

#include "TallyVectorPostprocessor.h"

template<>
InputParameters validParams<TallyVectorPostprocessor>()
{
  InputParameters params = validParams<GeneralVectorPostprocessor>();

  params.addRequiredParam<UserObjectName>("monte_carlo_userobject", "The MonteCarloUserObject to pull data from");

  return params;
}

TallyVectorPostprocessor::TallyVectorPostprocessor(const std::string & name, InputParameters parameters) :
    GeneralVectorPostprocessor(name, parameters),
    _monte_carlo_user_object(getUserObject<MonteCarloUserObject>("monte_carlo_userobject")),
    _collision_rate(declareVector("collision_rate")),
    _mean(declareVector("mean")),
    _variance(declareVector("variance"))
{
}

void
TallyVectorPostprocessor::initialize()
{}

void
TallyVectorPostprocessor::execute()
{
  const TallyGrid & tally_grid = _monte_carlo_user_object.getTallyGrid();

  _collision_rate = tally_grid.getCollisionTallies();
  _mean = tally_grid.getMean();
  _variance = tally_grid.getVariance();
}
