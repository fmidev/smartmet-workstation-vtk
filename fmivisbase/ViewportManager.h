#ifndef ViewportManager_h__
#define ViewportManager_h__

#include <vector>
#include <memory>

#include <vtkSmartPointer.h>

#include "VisualizerManager.h"

class vtkRenderer;
class vtkTextActor;

namespace fmiVis {
	class TimeAnimator;

	typedef size_t ViewportID;

	struct Viewport {
		vtkSmartPointer<vtkRenderer> r;
		std::unique_ptr<VisualizerManager> vm;
		std::unique_ptr< std::vector<vtkSmartPointer<vtkTextActor> > > vt;
		Viewport(vtkSmartPointer<vtkRenderer> r,
			std::unique_ptr<VisualizerManager> &&vm,
			std::unique_ptr< std::vector<vtkSmartPointer<vtkTextActor> > > &&vt) :
			r(r), vm(std::move(vm)), vt(std::move(vt)) 
		{}
	};

	//a rather transparent intermediate manager that holds one or more viewports, each with their own visualizermanager
	class ViewportManager {

	protected:
		std::vector<Viewport> viewports;

	public:
		virtual void UpdateTimeStep(double t);
		virtual void Update();

		virtual ViewportID whichViewport(vtkRenderer *vp);

		virtual void ViewportNumKey(ViewportID vpid, int vid, bool ctrl);

		virtual void AddViewport(vtkSmartPointer<vtkRenderer> r,
			std::unique_ptr<VisualizerManager> &&vm,
			std::unique_ptr<std::vector<vtkSmartPointer<vtkTextActor>>> &&vt);

		void ReloadOptions();
	};


}

#endif // ViewportManager_h__
