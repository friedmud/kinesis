#ifndef MONTECARLOUSEROBJECT_H
#define MONTECARLOUSEROBJECT_H

#include "GeneralUserObject.h"
#include "libmesh/fparser.hh"

//Forward Declarations
class MonteCarloUserObject;

template<>
InputParameters validParams<MonteCarloUserObject>();

/**
  * Monte Carlo particle simulation
  */
class MonteCarloUserObject : public GeneralUserObject
{
public:
  MonteCarloUserObject(const std::string & name, InputParameters parameters);
  virtual ~MonteCarloUserObject();

  virtual void execute();
  virtual void initialize() {};
  virtual void finalize() {};


protected:
};

#endif //MONTECARLOUSEROBJECT_H
