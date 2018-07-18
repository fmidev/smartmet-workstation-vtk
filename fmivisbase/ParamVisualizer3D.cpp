#include "ParamVisualizer3D.h"

#include <vtkContourFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkCleanPolyData.h>

#include <vtkSmartVolumeMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>

#include "newBaseSourcer.h"

void ParamVisualizer3D::ModeVolume() {

	contourFilter->RemoveAllInputConnections(0);

	volMap->RemoveAllInputConnections(0);
	volMap->SetInputConnection(nbs->GetOutputPort());

	filters.clear();

	SetActiveMapper(volMap);

	SetProp(volAct);
}

void ParamVisualizer3D::ModeContour() {
	volMap->RemoveAllInputConnections(0);
	contourFilter->SetInputConnection(nbs->GetOutputPort());

	filters.clear();

	filters.push_back(contourFilter);
	filters.push_back(cleanFilter);

	SetActiveMapper(polyMap);

	SetProp(polyAct);
}

ParamVisualizer3D::ParamVisualizer3D(const std::string & file, nbsMetadata & m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac, vtkSmartPointer<vtkColorTransferFunction> volumeColor, vtkSmartPointer<vtkPiecewiseFunction> volumeOpacity, float contourThreshold, double contourColor[3], float contourOpacity) :
	ParamVisualizerBase(file, m, paramIdent,fac)

{

	volMap = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	volMap->SetBlendModeToComposite();
	volMap->SetRequestedRenderModeToGPU();
	volMap->SetMaxMemoryInBytes(2e9);
	volMap->SetInteractiveUpdateRate(0.1);

	volProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volProperty->ShadeOff();

	volProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

	volProperty->SetColor(volumeColor);
	volProperty->SetScalarOpacity(volumeOpacity);


	volAct = vtkSmartPointer<vtkVolume>::New();

	volAct->SetMapper(volMap);
	volAct->SetProperty(volProperty);

	contourFilter = vtkSmartPointer<vtkContourFilter>::New();

	contourFilter->ComputeNormalsOn();
	contourFilter->ComputeScalarsOff();
	contourFilter->SetValue(0, contourThreshold);

	contourFilter->SetUseScalarTree(true);

	cleanFilter = vtkSmartPointer<vtkCleanPolyData>::New();

	cleanFilter->SetInputConnection(contourFilter->GetOutputPort());


	polyMap = vtkSmartPointer<vtkPolyDataMapper>::New();

	polyMap->SetInputConnection(cleanFilter->GetOutputPort());
	polyMap->ScalarVisibilityOff();

	polyAct = vtkSmartPointer<vtkActor>::New();

	polyAct->SetMapper(polyMap);
	polyAct->GetProperty()->SetColor(contourColor);
	polyAct->GetProperty()->SetOpacity(contourOpacity);
	polyAct->GetProperty()->FrontfaceCullingOff();
	polyAct->GetProperty()->BackfaceCullingOff();

	polyAct->SetBackfaceProperty(polyAct->GetProperty());
	polyAct->GetBackfaceProperty()->SetOpacity(1);

	mode = false;
	ModeVolume();
}

