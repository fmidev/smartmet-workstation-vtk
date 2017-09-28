#include "ParamVisualizerBase.h"

#include <vtkAbstractMapper.h>

#include <vtkColorTransferFunction.h>
#include <vtkRenderer.h>

#include "newBaseSourcer.h"


#include "AreaUtil.h"


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
	UpdateNBS(t);

	for (auto &filter : filters) {
		//cout << "Updating filter " << filter->GetClassName() << endl;
		filter->UpdateTimeStep(t);
	}
	activeMapper->UpdateTimeStep(t);
}

void ParamVisualizerBase::UpdateNBS(double t)
{

	auto areaExt = AreaUtil::FindExtentScandic(ren, meta.dataInfo->Area());
	int extents[6] = { areaExt[0],areaExt[1],
					areaExt[2],areaExt[3],
					1,1 };
	nbs->Modified();
	nbs->UpdateTimeStep(t,-1,1,0,extents);
}
