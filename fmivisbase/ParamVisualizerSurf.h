#ifndef PARAMVISUALIZERSURF_H
#define PARAMVISUALIZERSURF_H

#include <memory>
#include <vtkSmartPointer.h>

#include "ParamVisualizerBase.h"

class vtkPolyDataMapper;

class vtkContourFilter;
class vtkScalarsToColors;
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

class NFmiDrawParam;
class NFmiDataIdent;

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

	vtkSmartPointer<vtkScalarsToColors> isolineColFunc;
	vtkSmartPointer<vtkScalarsToColors> contourColFunc;

	//false = color
	bool mode;

	void ModeIsoLine();
	void ModeColorContour();


public:
	ParamVisualizerSurf(const std::string &file, nbsMetadata &m, ContourLabeler &labeler, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac, bool flat = false);

	void ReloadOptions();

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