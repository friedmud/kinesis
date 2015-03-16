#include "KinesisApp.h"
#include "Moose.h"
#include "AppFactory.h"

// UserObjects
#include "MonteCarloUserObject.h"

// VectorPostprocessors
#include "TallyVectorPostprocessor.h"

template<>
InputParameters validParams<KinesisApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

KinesisApp::KinesisApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  KinesisApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  KinesisApp::associateSyntax(_syntax, _action_factory);
}

KinesisApp::~KinesisApp()
{
}

extern "C" void KinesisApp__registerApps() { KinesisApp::registerApps(); }
void
KinesisApp::registerApps()
{
  registerApp(KinesisApp);
}

void
KinesisApp::registerObjects(Factory & factory)
{
  registerUserObject(MonteCarloUserObject);

  registerVectorPostprocessor(TallyVectorPostprocessor);
}

void
KinesisApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
