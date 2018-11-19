#ifndef ViewportFactory_h__
#define ViewportFactory_h__

#include <memory>
#include <map>
#include <vector>

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include "ParamVisualizerBase.h"
#include "Interactor.h"
#include "VisualizerManager.h"


class vtkRenderWindow;
class vtkRenderWindowInteractor;


//here are some methods to handle all the setup to get visualizer system up and running
namespace fmiVis {

	struct nbsMetadata;

	class ViewportManager;
	class ViewportManagerTimegrid;
	class VisualizationInteractor2D;

	class ViewportFactory {
	public:
		static std::map<int, std::string> paramsSurf;

		//sets up numX by numY viewports onto the viewportMan and builds the UI. The data is sourced from file and drawn with options from drawParamPath
		static void MakeTimeGridView(size_t numX, size_t numY,std::string file, std::string drawParamPath,nbsMetadata &meta, NFmiFastQueryInfo &dataInfo,ViewportManagerTimegrid &viewportMan,
			vtkRenderWindowInteractor *iren, vtkRenderWindow *renWin, VisualizationInteractor2D *style);

	};

	void AddMapPlane(vtkSmartPointer<vtkRenderer> ren, const std::string &file, nbsMetadata &meta, bool flat = true);
	void MakeTimeAnimator(vtkRenderer *ren, fmiVis::ViewportManager &vm, nbsMetadata &meta,
		vtkRenderWindowInteractor *iren, vtkRenderWindow *renWin, VisualizationInteractorImpl &style);
	void MakeFileText(std::string &file, vtkSmartPointer<vtkRenderer> ren);
	visID addVis(std::unique_ptr<ParamVisualizerBase> vis, std::string printName,
		VisualizerManager &vm,
		std::vector<vtkSmartPointer<vtkTextActor> > &textActs, vtkRenderer *ren,
		int textSize, double textVOff, double textVSpacing);
}


#endif // ViewportFactory_h__