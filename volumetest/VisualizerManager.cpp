#include "VisualizerManager.h"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include "ParamVisualizerBase.h"

#include "nbsMetadata.h"

#include "ContourLabeler.h"

VisualizerManager::VisualizerManager(vtkSmartPointer<vtkRenderer> ren, float zHeight /*= 13000*/) :
	renderer(ren),
	visualizers(),
	labeler(std::make_unique<ContourLabeler>(ren)),
	meta(),
	prevTime(0)
{
	meta.maxH = zHeight;
}
VisualizerManager::~VisualizerManager() {};
visID VisualizerManager::AddVisualizer(std::unique_ptr<ParamVisualizerBase> v)
{
	visID vid = visualizers.size();
	if (v)
	{
		visualizers.push_back(std::move(v));
		visualizers[vid]->DisableActor();
		return vid;
	}
	else return -1;
}

void VisualizerManager::EnableVis(visID vid)
{
	if (vid < visualizers.size())
		visualizers[vid]->EnableActor();
	Update();
}

void VisualizerManager::DisableVis(visID vid)
{
	if (vid < visualizers.size())
		visualizers[vid]->DisableActor();
	Update();
}

void VisualizerManager::ToggleVis(visID vid)
{
	if (vid < visualizers.size())
		if (visualizers[vid]->IsEnabled())
			visualizers[vid]->DisableActor();
		else
			visualizers[vid]->EnableActor();
	Update();
}

bool VisualizerManager::IsEnabled(visID vid)
{
	if (vid < visualizers.size())
		return visualizers[vid]->IsEnabled();
	return false;
}

void VisualizerManager::SetCrop(visID vid, bool c)
{
	if (vid < visualizers.size())
		visualizers[vid]->SetCropping(c);
	Update();
}

void VisualizerManager::CropMappers(vtkPlanes* p)
{
	for (auto &vis : visualizers) {
		vis->CropMapper(p);
	}
	Update();
}

void VisualizerManager::ToggleMode(visID vid)
{
	if (vid < visualizers.size())
		visualizers[vid]->ToggleMode();
	Update();
}

void VisualizerManager::UpdateTimeStep(double t)
{
	labeler->Clear();
	prevTime = t;
	for (auto &vis : visualizers) {
		if (vis->IsEnabled())
			vis->UpdateTimeStep(t);
		renderer->AddViewProp(vis->GetProp());
	}
	if(visualizers.size()>0)
		labeler->Update();
	renderer->GetRenderWindow()->Render();
}

int VisualizerManager::GetVisParam(visID vid)
{
	if (vid < visualizers.size())
		return visualizers[vid]->param;
	return -1;
}
