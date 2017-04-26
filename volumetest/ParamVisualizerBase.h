#ifndef PARAMVISUALIZERBASE_H
#define PARAMVISUALIZERBASE_H


#include <vtkSmartPointer.h>
#include <vtkProp.h>
#include <vtkAbstractMapper.h>
#include <vtkAlgorithm.h>
#include <vtkPlanes.h>

#include "newBaseSourcerMetaData.h"
#include "newBaseSourcer.h"


class ParamVisualizerBase {
private:
	vtkAbstractMapper *activeMapper;
	vtkProp *prop;
	bool crop;
	bool enabled;

protected:

	const metaData &meta;

	std::list<vtkAlgorithm* > filters;

	newBaseSourcer* nbs;

	inline void SetActiveMapper(vtkAbstractMapper *m) {
		if(activeMapper) m->SetClippingPlanes(activeMapper->GetClippingPlanes());
		activeMapper = m;
	}
	inline void SetProp(vtkProp *p) {
		if(prop) prop->SetVisibility(false);
		prop = p;
		prop->SetVisibility(true);
	}


public:
	int param;

	ParamVisualizerBase(const std::string &file, metaData &m, int param) :
		meta(m), activeMapper(nullptr),
		filters(), nbs(new newBaseSourcer(file, &m, param ) ),
		prop(nullptr), crop(true), enabled(true), param(param)
	{
	}
	inline virtual ~ParamVisualizerBase() {
		nbs->Delete();
	}
	inline void CropMapper(vtkPlanes* p) {
		if(crop)
			activeMapper->SetClippingPlanes(p);

	}
	inline void SetCropping(bool c) {
		crop = c;
	}

	inline virtual void UpdateTimeStep(double t) {
		nbs->UpdateTimeStep(t);
		for (auto &filter : filters) {
			filter->UpdateTimeStep(t);
		}
	}

	inline void EnableActor() {
		enabled = true;

		if (prop) prop->SetVisibility(true);
	}

	inline void DisableActor() {
		enabled = false;

		if (prop) prop->SetVisibility(false);

	}

	inline bool IsEnabled() {
		return enabled;
	}

	inline vtkProp* GetProp() {
		return prop;
	}

	virtual void ToggleMode() = 0;
};

#endif /* PARAMVISUALIZERBASE_H */