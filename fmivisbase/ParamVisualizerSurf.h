#ifndef PARAMVISUALIZERSURF_H
#define PARAMVISUALIZERSURF_H

#include <memory>
#include <vtkSmartPointer.h>

#include "ParamVisualizerBase.h"

class vtkPolyDataMapper;

class vtkContourFilter;
class vtkColorTransferFunction;
class vtkStripper;
class vtkActor;
class vtkPolyData;
class ContourLabeler;
class vtkAppendPolyData;
class vtkClipPolyData;
class HatchSource;
class vtkImplicitSelectionLoop;
class vtkContourTriangulator;
class vtkDecimatePolylineFilter;
class vtkCleanPolyData;

class ParamVisualizerSurf : public ParamVisualizerBase {
protected:

	HatchSource *hatch;
	vtkClipPolyData *clip;
	vtkImplicitSelectionLoop *loop;

	vtkContourFilter *contour;

	vtkStripper* stripper;

	ContourLabeler &labeler;


	vtkAppendPolyData *append;

	vtkSmartPointer<vtkContourTriangulator> triangulate;
	vtkSmartPointer<vtkDecimatePolylineFilter> decimate;
	vtkSmartPointer<vtkCleanPolyData> clean;

	vtkPolyDataMapper *polyMap;

	vtkActor *polyAct;

	//false = color
	bool mode;

	void ModeIsoLine();
	void ModeColorContour();


public:
	ParamVisualizerSurf(const std::string &file, nbsMetadata &m, int param,
		vtkSmartPointer<vtkColorTransferFunction> contourColors, ContourLabeler &labeler, double range[2], int numContours, bool flat = false);
	~ParamVisualizerSurf();

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

#endif /*PARAMVISUALIZERSURF_H*/