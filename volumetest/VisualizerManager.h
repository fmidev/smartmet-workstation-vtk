#ifndef VISUALIZERMANAGER_H
#define VISUALIZERMANAGER_H

#include <memory>
#include <vector>

#include <vtkSmartPointer.h>

#include "nbsMetadata.h"

#include "ParamVisualizerBase.h"

class vtkRenderer;
class ContourLabeler;

struct nbsMetadata;
class vtkPlanes;

typedef size_t visID;

class VisualizerManager {
	std::vector<std::unique_ptr<ParamVisualizerBase> > visualizers;
	vtkSmartPointer<vtkRenderer> renderer;
	std::shared_ptr<ContourLabeler> labeler;

	nbsMetadata meta;

	double prevTime;


public:

	VisualizerManager(vtkSmartPointer<vtkRenderer> ren, float zHeight = 13000);
	~VisualizerManager();
	inline nbsMetadata& GetMeta() {
		return meta;
	}

	visID AddVisualizer(std::unique_ptr<ParamVisualizerBase> v);

	inline int GetVisNum() {
		return visualizers.size();
	}

	void EnableVis(visID vid);
	void DisableVis(visID vid);
	void ToggleVis(visID vid);
	bool IsEnabled(visID vid);
	void SetCrop(visID vid,bool c);

	void CropMappers(vtkPlanes* p);

	void ToggleMode(visID vid);

	void UpdateTimeStep(double t);
	inline void Update() {
		UpdateTimeStep(prevTime);
	}
	int GetVisParam(visID vid);

	inline std::shared_ptr<ContourLabeler> GetLabeler() {
		return labeler;
	}
};

#endif /* VISUALIZERMANAGER_H */