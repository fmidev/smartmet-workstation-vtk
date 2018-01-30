#ifndef PARAMVISUALIZER2D_H
#define PARAMVISUALIZER2D_H

#include <memory>
#include <vtkSmartPointer.h>

#include "ParamVisualizerBase.h"

class vtkPolyDataMapper;
class vtkProbeFilter;
class vtkContourFilter;
class vtkColorTransferFunction;
class vtkStripper;
class vtkActor;
class vtkAlgorithmOutput;

class ContourLabeler;

class ParamVisualizer2D : public ParamVisualizerBase {
protected:



	vtkSmartPointer<vtkPolyDataMapper> polyMap;
	
	vtkSmartPointer<vtkProbeFilter> probeFilter;
	vtkSmartPointer<vtkContourFilter> contourFilter;

	vtkSmartPointer<vtkStripper> contourStripper;

	ContourLabeler &labeler;

	vtkSmartPointer<vtkActor>polyAct;

	//false = color
	bool mode;

	void ModeIsoLine();
	void ModeColorContour();


public:
	ParamVisualizer2D(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac, vtkAlgorithmOutput* probingData,
		vtkSmartPointer<vtkColorTransferFunction> contourColors, ContourLabeler &labeler, double range[2], int numContours);

	virtual void UpdateTimeStep(double t) override;

	vtkScalarsToColors  * getColor();
	double * getRange();

	virtual inline void ToggleMode() {

		if (mode) {
			ModeColorContour();
		}
		else {
			ModeIsoLine();
		}

		mode = !mode;
	}
};

#endif /*PARAMVISUALIZER2D_H*/