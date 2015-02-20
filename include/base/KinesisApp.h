#ifndef KINESISAPP_H
#define KINESISAPP_H

#include "MooseApp.h"

class KinesisApp;

template<>
InputParameters validParams<KinesisApp>();

class KinesisApp : public MooseApp
{
public:
  KinesisApp(const std::string & name, InputParameters parameters);
  virtual ~KinesisApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* KINESISAPP_H */
