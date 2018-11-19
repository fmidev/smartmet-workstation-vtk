#ifndef VISUALIZERMANAGER_H
#define VISUALIZERMANAGER_H

#include <memory>
#include <vector>

#include <vtkSmartPointer.h>

#include "nbsMetadata.h"

#include "ParamVisualizerBase.h"


class vtkRenderer;
class vtkPlanes;
class vtkScalarBarWidget;





namespace fmiVis {


	struct nbsMetadata;
	class ContourLabeler;

	typedef size_t visID;




	//a manager class that owns and collects the actors from paramvisualizers
	//also owns the contourlabeler and creates a color legend
	//each visualizer is associated with their visID, returned by AddVisualizer()
	class VisualizerManager {

		std::vector<std::unique_ptr<ParamVisualizerBase> > visualizers;

		vtkRenderer *renderer;

		std::unique_ptr<ContourLabeler> labeler;

		vtkSmartPointer<vtkScalarBarWidget> legend;

		nbsMetadata &meta;

		double prevTime;

		std::shared_ptr<NFmiDrawParamFactory> drawParamFac;


	public:

		VisualizerManager(vtkRenderer *ren, std::string drawParamPath, nbsMetadata &m, float zHeight = 13000);
		~VisualizerManager();

		inline nbsMetadata& GetMeta() {
			return meta;
		}

		void setDrawParamFac(std::shared_ptr<NFmiDrawParamFactory> fac) {
			drawParamFac = fac;
		}

		visID AddVisualizer(std::unique_ptr<ParamVisualizerBase> v);

		inline int GetVisNum() {
			return visualizers.size();
		}

		void EnableVis(visID vid);
		void DisableVis(visID vid);
		void ToggleVis(visID vid);
		bool IsEnabled(visID vid);
		void SetCrop(visID vid, bool c);

		void CropMappers(vtkPlanes* p);

		void ToggleMode(visID vid);

		void ReloadOptions(visID vid = -1); //-1 for all

		void UpdateTimeStep(double t);

		inline void Update() {
			UpdateTimeStep(prevTime);
		}

		//what parameter is vid visualizing
		int GetVisParam(visID vid);

		inline ContourLabeler& GetLabeler() {
			return *labeler;
		}

	};

}
#endif /* VISUALIZERMANAGER_H */