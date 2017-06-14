#include "ParamVisualizerBase.h"

#include <vtkAbstractMapper.h>


#include "vtkColorTransferFunction.h"


#include "newBaseSourcer.h"



ParamVisualizerBase::ParamVisualizerBase(const std::string &file, nbsMetadata &m, int param) :
	meta(m), activeMapper(nullptr),
	filters(), nbs(new newBaseSourcer(file, &m, param)),
	prop(nullptr), crop(true), enabled(false), param(param)
{
	cout << "Initializing visualizer for param " << param << endl;
}

ParamVisualizerBase::ParamVisualizerBase( vtkAlgorithm *nbs, nbsMetadata &m, int param) :
	nbs(nbs), meta( m ), activeMapper(nullptr),
	filters(), prop(nullptr), crop(true), enabled(false), param(param)
{

	cout << "Initializing visualizer for param " << param << endl;
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

	bool vis = false;
	if (prop){
		vis = prop->GetVisibility();
		prop->SetVisibility(false);
	}
	prop = p;
	prop->SetVisibility(vis);
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
		//cout << "Updating filter " << filter->GetClassName() << endl;
		filter->UpdateTimeStep(t);
	}
	activeMapper->UpdateTimeStep(t);
}
