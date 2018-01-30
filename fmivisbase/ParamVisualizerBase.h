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
class vtkRenderer;
class NFmiDrawParamFactory;
class NFmiDataIdent;

class ParamVisualizerBase {
private:
	vtkAbstractMapper *activeMapper;
	vtkProp *prop;
	bool crop;
	bool enabled;

	vtkRenderer * ren;


protected:


	NFmiDrawParamFactory* drawParamFac = nullptr;
	
	NFmiDataIdent &paramIdent;

	const nbsMetadata &meta;

	std::list<vtkAlgorithm* > filters;

	vtkAlgorithm* nbs;

	void SetActiveMapper(vtkAbstractMapper *m);
	void SetProp(vtkProp *p);
public:
	int param;

	ParamVisualizerBase(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent,NFmiDrawParamFactory* fac);
	ParamVisualizerBase(vtkAlgorithm *nbs, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac);
	virtual ~ParamVisualizerBase();
	void CropMapper(vtkPlanes* p);

	inline void SetCropping(bool c) {
		crop = c;
	}


	inline void SetRenderer(vtkRenderer* r) {
		ren = r;
	}

	virtual void UpdateTimeStep(double t);

	void UpdateNBS(double t);

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

	inline void SetDrawParamFac(NFmiDrawParamFactory *p) {
		drawParamFac = p;
	}

	virtual void ToggleMode() {};

};

#endif /* PARAMVISUALIZERBASE_H */