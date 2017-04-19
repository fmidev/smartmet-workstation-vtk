#ifndef PARAMVISUALIZER2D_H
#define PARAMVISUALIZER2D_H


#include <vtkSmartVolumeMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkContourFilter.h>
#include <vtkProbeFilter.h>


#include <vtkColorTransferFunction.h>

#include <vtkActor.h>

#include "ParamVisualizerBase.h"


class ParamVisualizer2D : public ParamVisualizerBase {
protected:
	vtkPolyDataMapper *polyMap;
	
	vtkProbeFilter *probeFilter;
	vtkContourFilter *contourFilter;

	vtkActor *polyAct;

	//false = volume
	bool mode;

	void ModeIsoLine();
	void ModeColorContour();



public:
	ParamVisualizer2D(const std::string &file, metaData &m, int param, vtkAlgorithmOutput* probingData,
		vtkSmartPointer<vtkColorTransferFunction> contourColors, double range[2], int numContours);
	~ParamVisualizer2D();



	virtual inline void ToggleMode() {

		if (mode) {
			ModeIsoLine();
		}
		else {
			ModeColorContour();
		}

		mode = !mode;
	}
};

#endif /*PARAMVISUALIZER2D_H*/