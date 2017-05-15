#include "ParamVisualizerBase.h"

#include <vtkAbstractMapper.h>
#include "newBaseSourcer.h"


ParamVisualizerBase::ParamVisualizerBase(const std::string &file, nbsMetadata &m, int param) :
	meta(m), activeMapper(nullptr),
	filters(), nbs(new newBaseSourcer(file, &m, param)),
	prop(nullptr), crop(true), enabled(true), param(param)
{

}
ParamVisualizerBase::ParamVisualizerBase(newBaseSourcer *nbs) :
	nbs(nbs), meta( nbs->getMeta() ), activeMapper(nullptr),
	filters(), prop(nullptr), crop(true), enabled(true), param(param)
{
}

ParamVisualizerBase::~ParamVisualizerBase()
{
	nbs->Delete();
}


void ParamVisualizerBase::SetActiveMapper(vtkAbstractMapper *m)
{
	if (activeMapper) m->SetClippingPlanes(activeMapper->GetClippingPlanes());
	activeMapper = m;
}

void ParamVisualizerBase::SetProp(vtkProp *p)
{
	if (prop) prop->SetVisibility(false);
	prop = p;
	prop->SetVisibility(true);
}

void ParamVisualizerBase::CropMapper(vtkPlanes* p)
{
	if (crop)
		activeMapper->SetClippingPlanes(p);
}

void ParamVisualizerBase::UpdateTimeStep(double t)
{
	nbs->UpdateTimeStep(t);
	for (auto &filter : filters) {
		filter->UpdateTimeStep(t);
	}
}
