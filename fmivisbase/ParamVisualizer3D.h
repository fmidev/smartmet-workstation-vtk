#ifndef PARAMVISUALIZER3D_H
#define PARAMVISUALIZER3D_H

#include <vtkSmartPointer.h>

#include "ParamVisualizerBase.h"

class vtkPolyDataMapper;
class vtkSmartVolumeMapper;
class vtkContourFilter;
class vtkCleanPolyData;
class vtkColorTransferFunction;
class vtkVolumeProperty;
class vtkPiecewiseFunction;
class vtkVolume;
class vtkActor;



class ParamVisualizer3D : public ParamVisualizerBase {
protected:
	vtkSmartPointer<vtkPolyDataMapper> polyMap;
	vtkSmartPointer<vtkSmartVolumeMapper> volMap;

	vtkSmartPointer<vtkContourFilter> contourFilter;
	vtkSmartPointer<vtkCleanPolyData> cleanFilter;

	vtkSmartPointer<vtkVolumeProperty> volProperty;

	vtkSmartPointer<vtkVolume> volAct;
	vtkSmartPointer<vtkActor> polyAct;

	//false = volume
	bool mode;

	void ModeVolume();
	void ModeContour();

public:
	ParamVisualizer3D(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac,
		vtkSmartPointer<vtkColorTransferFunction> volumeColor, vtkSmartPointer<vtkPiecewiseFunction> volumeOpacity,
		float contourThreshold, double contourColor[3], float contourOpacity);

	virtual inline void ToggleMode() {

		if (mode) {
			ModeVolume();
		}
		else {
			ModeContour();
		}

		mode = !mode;
	}
};

#endif /*PARAMVISUALIZER3D_H*/