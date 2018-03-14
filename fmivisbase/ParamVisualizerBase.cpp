#include "ParamVisualizerBase.h"

#include <vtkAbstractMapper.h>

#include <vtkColorTransferFunction.h>
#include <vtkRenderer.h>

#include <NFmiDataIdent.h>

#include "newBaseSourcer.h"


#include "AreaUtil.h"


ParamVisualizerBase::ParamVisualizerBase(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac) :
	meta(m), activeMapper(nullptr),
	filters(), nbs(new newBaseSourcer(file, &m, paramIdent.GetParamIdent())),
	prop(nullptr), crop(true), enabled(false), paramIdent(paramIdent), param(paramIdent.GetParamIdent()), drawParamFac(fac)
{
	cout << "Initializing visualizer for param " << paramIdent << endl;
}

ParamVisualizerBase::ParamVisualizerBase( vtkAlgorithm *nbs, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac) :
	nbs(nbs), meta( m ), activeMapper(nullptr),
	filters(), prop(nullptr), crop(true), enabled(false),paramIdent(paramIdent), param(paramIdent.GetParam()->GetIdent()), drawParamFac(fac)
{

	cout << "Initializing visualizer for param " << paramIdent << endl;
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
	UpdateNBS(t);

	for (auto &filter : filters) {
		//cout << "Updating filter " << filter->GetClassName() << endl;
		filter->UpdateTimeStep(t);
	}
	activeMapper->UpdateTimeStep(t);
}

void ParamVisualizerBase::UpdateNBS(double t)
{
	nbs->Modified();
	nbs->UpdateTimeStep(t);
}
