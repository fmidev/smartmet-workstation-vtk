#ifndef PARAMVISUALIZERSURF_H
#define PARAMVISUALIZERSURF_H

#include <memory>
#include <vtkSmartPointer.h>
#include <map>

#include "ParamVisualizerBase.h"

class vtkPolyDataMapper;

class vtkMarchingContourFilter;
class vtkScalarsToColors;


class vtkStripper;
class vtkContourLoopExtraction;
class vtkCookieCutter;
class vtkRibbonFilter;

class vtkActor;
class vtkPolyData;
class vtkAppendPolyData;
class vtkClipPolyData;
class vtkImplicitSelectionLoop;
class vtkContourTriangulator;
class vtkDecimatePolylineFilter;
class vtkCleanPolyData;
class vtkFloatArray;
class vtkSplineFilter;

class NFmiDrawParam;
class NFmiDataIdent;

namespace fmiVis {

	class HatchSource;
	class ContourLabeler;
	//a bloated class for visualizing surface data - supports option reloading, line widths and colors
	//much of the code is duplicate from visualizer2D
	//TODO merge with paramvisualizer2D
	//TODO hatch drawing - lots of artifacts of past attempts at it remain
	//vtkContourTriangualator could get the job done if it did what it claimed - at the time of writing, it does not clean up the lines like the documentation says
	class ParamVisualizerSurf : public ParamVisualizerBase {
	protected:
		
		//hatching
		HatchSource *hatch;
		vtkContourLoopExtraction *loop;
		vtkCookieCutter *cutter;

		vtkMarchingContourFilter *contour;

		vtkStripper* stripper;

		vtkRibbonFilter *ribbon;
		vtkSmartPointer<vtkFloatArray> pointWidths;

		vtkSplineFilter *smooth;

		ContourLabeler &labeler;

		boost::shared_ptr<NFmiDrawParam> drawParam;

		bool flat;

		vtkAppendPolyData *append;

		//hatching attempts
		vtkSmartPointer<vtkContourTriangulator> triangulate;
		vtkSmartPointer<vtkDecimatePolylineFilter> decimate;
		vtkSmartPointer<vtkCleanPolyData> clean;

		vtkPolyDataMapper *polyMap;

		vtkActor *polyAct;

		vtkSmartPointer<vtkScalarsToColors> isolineColFunc;
		vtkSmartPointer<vtkScalarsToColors> contourColFunc;

		//isoline value : line width
		std::map<double, double> lineWidth;

		//false = color
		bool mode;

		void ModeIsoLine();
		void ModeColorContour();


	public:
		//flat is passed to nbs and controls if the mesh gets height data applied to it
		ParamVisualizerSurf(const std::string &file, nbsMetadata &m, ContourLabeler &labeler, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac, bool flat = false);

		//reads the options from a smartmet settings folder
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

		virtual void UpdateNBS(double t) override;
	};

}

#endif /*PARAMVISUALIZERSURF_H*/