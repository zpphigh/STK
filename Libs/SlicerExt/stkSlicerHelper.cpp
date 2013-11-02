#include "stkSlicerHelper.h"

// Qt includes
#include <QList>
#include <QSettings>
#include <QString>

// Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerApplicationHelper.h"
#include "qSlicerCommandOptions.h"
#include "qSlicerCoreModuleFactory.h"
#include "qSlicerLoadableModuleFactory.h"
#include "qSlicerModuleFactoryManager.h"
#include "qSlicerModuleManager.h"

// ITK includes
#include <itkConfigure.h> // For ITK_VERSION_MAJOR
#include <itkFactoryRegistration.h>


// You should launch a slicer application as following sequence, or you will get a NullPtr error
//	1. SetupModules
//	2. New window: new ImagingApp;
//	3. LoadModules
//	4. Display window:imagingApp->show();

//----------------------------------------------------------------------------
void stkSlicerHelper::SetupModules()
{
	itk::itkFactoryRegistration(); 

	qSlicerApplication* app = qSlicerApplication::application();
	qSlicerModuleManager * moduleManager = qSlicerApplication::application()->moduleManager();
	qSlicerModuleFactoryManager * moduleFactoryManager = moduleManager->factoryManager();

	moduleFactoryManager->addSearchPaths(app->commandOptions()->additonalModulePaths());
	qSlicerApplicationHelper::setupModuleFactoryManager(moduleFactoryManager);

	moduleFactoryManager->registerModules();
	moduleFactoryManager->instantiateModules();
}

void stkSlicerHelper::LoadModules()
{
	qSlicerApplication* app = qSlicerApplication::application();
	qSlicerModuleManager * moduleManager = qSlicerApplication::application()->moduleManager();
	qSlicerModuleFactoryManager * moduleFactoryManager = moduleManager->factoryManager();

	foreach(const QString& name, moduleFactoryManager->instantiatedModuleNames())
	{
		Q_ASSERT(!name.isNull());
		moduleFactoryManager->loadModule(name);
	}
}