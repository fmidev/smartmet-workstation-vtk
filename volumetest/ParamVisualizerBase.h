#ifndef PARAMVISUALIZERBASE_H
#define PARAMVISUALIZERBASE_H

#include <memory>
#include <list>
#include <vtkSmartPointer.h>

#include <vtkProp.h>

#include "nbsMetadata.h"

class vtkAbstractMapper;
class newBaseSourcer;
class vtkAlgorithm;
class vtkPlanes;

class ParamVisualizerBase {
private:
	vtkAbstractMapper *activeMapper;
	vtkProp *prop;
	bool crop;
	bool enabled;

protected:

	const nbsMetadata &meta;

	std::list<vtkAlgorithm* > filters;

	newBaseSourcer* nbs;

	void SetActiveMapper(vtkAbstractMapper *m);
	void SetProp(vtkProp *p);


public:
	int param;

	ParamVisualizerBase(const std::string &file, nbsMetadata &m, int param);
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

	inline bool IsEnabled() {
		return enabled;
	}

	inline vtkProp* GetProp() {
		return prop;
	}

	virtual void ToggleMode() = 0;
};

#endif /* PARAMVISUALIZERBASE_H */