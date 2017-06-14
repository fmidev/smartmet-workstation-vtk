#include "ParamVisualizer3D.h"

#include <vtkContourFilter.h>
#include <vtkPolyDataNormals.h>;
#include <vtkCleanPolyData.h>;

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

ParamVisualizer3D::ParamVisualizer3D(const std::string & file, nbsMetadata & m, int param, vtkSmartPointer<vtkColorTransferFunction> volumeColor, vtkSmartPointer<vtkPiecewiseFunction> volumeOpacity, float contourThreshold, double contourColor[3], float contourOpacity) :
	ParamVisualizerBase(file, m, param),
	polyMap(vtkPolyDataMapper::New()),
	volMap(vtkSmartVolumeMapper::New()),
	contourFilter(vtkContourFilter::New()),
	cleanFilter(vtkCleanPolyData::New()),
	volProperty(vtkVolumeProperty::New()),
	volAct(vtkVolume::New()),
	polyAct(vtkActor::New())
{
	volMap->SetBlendModeToComposite();
	volMap->SetRequestedRenderModeToGPU();
	volMap->SetMaxMemoryInBytes(2e9);

	volProperty->ShadeOff();

	volProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

	volProperty->SetColor(volumeColor);
	volProperty->SetScalarOpacity(volumeOpacity);

	volAct->SetMapper(volMap);
	volAct->SetProperty(volProperty);

	contourFilter->ComputeNormalsOn();
	contourFilter->ComputeScalarsOff();
	contourFilter->SetValue(0, contourThreshold);

	contourFilter->SetUseScalarTree(true);

	cleanFilter->SetInputConnection(contourFilter->GetOutputPort());

	polyMap->SetInputConnection(cleanFilter->GetOutputPort());
	polyMap->ScalarVisibilityOff();

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

ParamVisualizer3D::~ParamVisualizer3D() {

	volProperty->Delete();
	volMap->Delete();
	volAct->Delete();

	contourFilter->Delete();
	polyMap->Delete();
	polyAct->Delete();

}
