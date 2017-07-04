#ifndef PARAMVISUALIZERBASE_H
#define PARAMVISUALIZERBASE_H

#include <memory>
#include <list>
#include <vtkSmartPointer.h>

#include <vtkProp.h>

#include "nbsMetadata.h"

class vtkAbstractMapper;
class vtkAlgorithm;
class vtkPlanes;
class vtkScalarsToColors;

class ParamVisualizerBase {
private:
	vtkAbstractMapper *activeMapper;
	vtkProp *prop;
	bool crop;
	bool enabled;

protected:

	const nbsMetadata &meta;

	std::list<vtkAlgorithm* > filters;

	vtkAlgorithm* nbs;

	void SetActiveMapper(vtkAbstractMapper *m);
	void SetProp(vtkProp *p);


public:
	int param;

	ParamVisualizerBase(const std::string &file, nbsMetadata &m, int param);
	ParamVisualizerBase(vtkAlgorithm *nbs, nbsMetadata &m, int param);
	virtual ~ParamVisualizerBase();
	void CropMapper(vtkPlanes* p);

	inline void SetCropping(bool c) {
		crop = c;
	}

	virtual void UpdateTimeStep(double t);

	inline void EnableActor() {
		enabled = true;

		if (prop) prop->SetVisibility(true);
	}

	inline void DisableActor() {
		enabled = false;

		if (prop) prop->SetVisibility(false);

	}

	virtual vtkScalarsToColors  * getColor() { return nullptr;  }
	virtual double * getRange() { return nullptr; }

	inline bool IsEnabled() {
		return enabled;
	}

	inline vtkProp* GetProp() {
		return prop;
	}

	virtual void ToggleMode() {};
};

#endif /* PARAMVISUALIZERBASE_H */