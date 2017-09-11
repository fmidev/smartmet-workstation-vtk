#ifndef ViewportManager_h__
#define ViewportManager_h__

#include <vector>
#include <memory>

#include <vtkSmartPointer.h>

class VisualizerManager;
class vtkRenderer;
class vtkTextActor;

class TimeAnimator;

namespace fmiVis {


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

	class ViewportManager {

		std::vector<Viewport> viewports;

	public:
		void UpdateTimeStep(double t);

		ViewportID whichViewport(vtkRenderer *vp);

		void ViewportNumKey(ViewportID vpid, int vid, bool ctrl);

		void AddViewport(vtkSmartPointer<vtkRenderer> r,
			std::unique_ptr<VisualizerManager> &&vm,
			std::unique_ptr<std::vector<vtkSmartPointer<vtkTextActor>>> &&vt);


	};


}

#endif // ViewportManager_h__
