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

namespace fmiVis {

	class ContourLabeler;

	//a 2D specialization of paramvisualizer to be used with the plane widget - samples the plane and draws it as color contours or isolines
	//TODO merge with ParamVisualizerSurf
	//TODO add functionality to draw both isolines and colors at the same time - mapping the colors will be nontrivial
	class ParamVisualizer2D : public ParamVisualizerBase {
	protected:


		vtkSmartPointer<vtkPolyDataMapper> polyMap;

		//probefilter samples probingData for the data to be drawn
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
			vtkSmartPointer<vtkScalarsToColors> contourColors, ContourLabeler &labeler, double range[2], int numContours);

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

}

#endif /*PARAMVISUALIZER2D_H*/