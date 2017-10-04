#ifndef fmiVisCameraCallback_h__
#define fmiVisCameraCallback_h__

#include <vtkCommand.h>
#include "ViewportManager.h"

namespace fmiVis {


	class CameraCallback : public vtkCommand
	{
	public:
		CameraCallback() {}

		static CameraCallback *New()
		{
			return new CameraCallback;
		}
		void Execute(vtkObject *caller, unsigned long, void*) VTK_OVERRIDE
		{
			vm->Update();

		}
		void setManager(ViewportManager *vm)
		{
			this->vm = vm;
		}


	protected:
		ViewportManager *vm;
	};
}


#endif // fmiVisCameraCallback_h__