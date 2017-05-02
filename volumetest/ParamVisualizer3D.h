#ifndef PARAMVISUALIZER3D_H
#define PARAMVISUALIZER3D_H

#include <vtkSmartPointer.h>

#include "ParamVisualizerBase.h"

class vtkPolyDataMapper;
class vtkSmartVolumeMapper;
class vtkContourFilter;
class vtkColorTransferFunction;
class vtkVolumeProperty;
class vtkPiecewiseFunction;
class vtkVolume;
class vtkActor;



class ParamVisualizer3D : public ParamVisualizerBase {
protected:
	vtkPolyDataMapper *polyMap;
	vtkSmartVolumeMapper *volMap;
	vtkContourFilter *contourFilter;

	vtkVolumeProperty *volProperty;

	vtkVolume *volAct;
	vtkActor *polyAct;

	//false = volume
	bool mode;

	void ModeVolume();
	void ModeContour();

public:
	ParamVisualizer3D(const std::string &file, nbsMetadata &m, int param,
		vtkSmartPointer<vtkColorTransferFunction> volumeColor, vtkSmartPointer<vtkPiecewiseFunction> volumeOpacity,
		float contourThreshold, double contourColor[3], float contourOpacity);
	~ParamVisualizer3D();

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