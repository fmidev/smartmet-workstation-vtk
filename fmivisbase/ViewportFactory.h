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


struct nbsMetadata;
class vtkRenderWindow;
class vtkRenderWindowInteractor;


namespace fmiVis {


	class ViewportManager;
	class ViewportManagerTimegrid;
	class VisualizationInteractor2D;

	class ViewportFactory {
	public:
		static std::map<int, std::string> paramsSurf;


		static void MakeTimeGridView(size_t numX, size_t numY,std::string file, std::string drawParamPath,nbsMetadata &meta,ViewportManagerTimegrid &viewportMan,
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