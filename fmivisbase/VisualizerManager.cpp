#include "VisualizerManager.h"


#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <vtkScalarBarWidget.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkScalarsToColors.h>
#include <vtkColorTransferFunction.h>

#include "ParamVisualizerBase.h"

#include "nbsMetadata.h"

#include "ContourLabeler.h"


#include "VisualizerFactory.h"

VisualizerManager::VisualizerManager(vtkRenderer *ren,nbsMetadata &m, float zHeight /*= 13000*/) :
	renderer(ren),
	visualizers(),
	labeler(std::make_unique<ContourLabeler>(ren)),
	legend(vtkSmartPointer<vtkScalarBarWidget>::New()),
	meta(m),
	prevTime(0)
{
	meta.maxH = zHeight;


	legend->GetScalarBarRepresentation()->SetPosition(0.92, 0.1);
	legend->GetScalarBarRepresentation()->SetPosition2(0.08, 0.8);
	legend->GetScalarBarRepresentation()->SetAutoOrient(false);


	legend->SetInteractor(ren->GetRenderWindow()->GetInteractor());


}
VisualizerManager::~VisualizerManager() {};
visID VisualizerManager::AddVisualizer(std::unique_ptr<ParamVisualizerBase> v)
{
	visID vid = visualizers.size();
	if (v)
	{
		v->SetRenderer(renderer);
		visualizers.push_back(std::move(v));
		visualizers[vid]->DisableActor();
		return vid;
	}
	else return -1;
}

void VisualizerManager::EnableVis(visID vid)
{
	if (vid < visualizers.size()) {

		visualizers[vid]->EnableActor();

		if (visualizers[vid]->getColor()) {
			legend->GetScalarBarActor()->SetLookupTable( visualizers[vid]->getColor() );
			legend->GetScalarBarActor()->GetLookupTable()->SetRange(visualizers[vid]->getRange());
			legend->EnabledOn();
		}

	}
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
			DisableVis(vid);
		else
			EnableVis(vid);
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
}

int VisualizerManager::GetVisParam(visID vid)
{
	if (vid < visualizers.size())
		return visualizers[vid]->param;
	return -1;
}
