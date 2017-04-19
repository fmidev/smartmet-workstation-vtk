#ifndef VISUALIZERMANAGER_H
#define VISUALIZERMANAGER_H

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include "ParamVisualizerBase.h"

#include "newBaseSourcerMetaData.h"

typedef size_t visID;

class VisualizerManager {
	std::vector<std::unique_ptr<ParamVisualizerBase> > visualizers;
	vtkSmartPointer<vtkRenderer> renderer;

	metaData meta;

	double prevTime;


public:

	VisualizerManager(vtkSmartPointer<vtkRenderer> ren, float zHeight = 13000) :
		renderer(ren),
		visualizers(),
		meta(),
		prevTime(0)
	{
		meta.maxH = zHeight;
	}
	~VisualizerManager() {
	}
	inline metaData& GetMeta() {
		return meta;
	}

	inline visID AddVisualizer(std::unique_ptr<ParamVisualizerBase> v) {
		visID vid = visualizers.size();
		if(v)
		{
			visualizers.push_back(std::move(v));

			return vid;
		}
		else return -1;
	}

	inline int GetVisNum() {
		return visualizers.size();
	}

	inline void EnableVis(visID vid) {
		if (vid<visualizers.size())
			visualizers[vid]->EnableActor();
		Update();
	}
	inline void DisableVis(visID vid) {
		if (vid<visualizers.size())
			visualizers[vid]->DisableActor();
		Update();
	}
	inline void ToggleVis(visID vid) {
		if (vid < visualizers.size())
			if (visualizers[vid]->IsEnabled())
				visualizers[vid]->DisableActor();
			else
				visualizers[vid]->EnableActor();
		Update();
	}
	inline bool IsEnabled(visID vid) {
		if (vid < visualizers.size())
			return visualizers[vid]->IsEnabled();
		return false;
			
	}
	inline void SetCrop(visID vid,bool c) {
		if (vid<visualizers.size())
			visualizers[vid]->SetCropping(c);
		Update();
	}

	inline void CropMappers(vtkPlanes* p) {
		for (auto &vis : visualizers) {
			vis->CropMapper(p);
		}
		Update();
	}

	inline void ToggleMode(visID vid) {
		if(vid<visualizers.size() )
			visualizers[vid]->ToggleMode();
		Update();
	}

	inline void UpdateTimeStep(double t) {
		prevTime = t;
		for (auto &vis : visualizers) {
			if(vis->IsEnabled())
				vis->UpdateTimeStep(t);
			renderer->AddViewProp(vis->GetProp());
		}
		renderer->GetRenderWindow()->Render();
	}
	inline void Update() {
		UpdateTimeStep(prevTime);
	}
	inline int GetVisParam(visID vid) {
		if (vid < visualizers.size())
			return visualizers[vid]->param;
	}
};

#endif /* VISUALIZERMANAGER_H */