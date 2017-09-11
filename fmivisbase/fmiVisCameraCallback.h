#ifndef fmiVisCameraCallback_h__
#define fmiVisCameraCallback_h__

#include <vtkCommand.h>
#include "VisualizerManager.h"

class fmiVisCameraCallback : public vtkCommand
{
public:
	fmiVisCameraCallback() {}

	static fmiVisCameraCallback *New()
	{
		return new fmiVisCameraCallback;
	}
	void Execute(vtkObject *caller, unsigned long, void*) VTK_OVERRIDE
	{
		vm->Update();

	}
	void setManager(VisualizerManager *vm)
	{
		this->vm = vm;
	}


protected:
	VisualizerManager *vm;
};


#endif // fmiVisCameraCallback_h__