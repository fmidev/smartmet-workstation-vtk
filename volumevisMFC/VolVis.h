#pragma once

#include <string.h>

#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

#include "nbsMetadata.h"
#include "ViewportManager.h"
#include "Interactor.h"
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPlaneWidget.h>
#include <vtkBoxWidget2.h>

#ifndef VolVis_h__
#define VolVis_h__

namespace  fmiVis {

	struct Vis3DParams {
		int param;
		vtkSmartPointer<vtkScalarsToColors> color;
		vtkSmartPointer<vtkPiecewiseFunction> opacity;
		double contourColor[3];
		double contourThreshold;
		double contourOpacity;

	};
	struct Vis2DParams {
		int param;
		vtkSmartPointer<vtkScalarsToColors> contourColors;
		double range[2];
		int numContours;

	};

	void InitConfig();


	struct Widgets3D {
		vtkSmartPointer<vtkPlaneWidget> plane;
		vtkSmartPointer<vtkBoxWidget2> box;
	};

	Widgets3D Make3DView(std::string file, std::string surfFile, std::string drawParamPath, nbsMetadata &meta, ViewportManager &viewportMan,
		vtkRenderWindowInteractor *iren, vtkRenderWindow *renWin, VisualizationInteractor *style);
}

#endif // VolVis_h__