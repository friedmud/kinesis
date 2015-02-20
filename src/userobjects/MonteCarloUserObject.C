#include "MonteCarloUserObject.h"
#include "MooseApp.h"
#include "Executioner.h"

#include "MonteCarloParticle.h"

template<>
InputParameters validParams<MonteCarloUserObject>()
{
  InputParameters params = validParams<GeneralUserObject>();

  params.addRequiredParam<unsigned int>("num_particles", "The total number of particles to track");

  return params;
}

MonteCarloUserObject::MonteCarloUserObject(const std::string & name, InputParameters parameters) :
    GeneralUserObject(name, parameters)
{
}

MonteCarloUserObject::~MonteCarloUserObject()
{
}

void
MonteCarloUserObject::execute()
{
}
