#include "ParamVisualizer2D.h"

#include <vtkActor.h>
#include <vtkProperty.h>

void ParamVisualizer2D::ModeIsoLine() {
	contourFilter->AddInputConnection(probeFilter->GetOutputPort(0));

	polyMap->RemoveAllInputConnections(0);
	polyMap->AddInputConnection(contourFilter->GetOutputPort());

	filters.clear();

	filters.push_back(probeFilter);
	filters.push_back(contourFilter);

}

void ParamVisualizer2D::ModeColorContour() {

	contourFilter->RemoveAllInputConnections(0);

	polyMap->RemoveAllInputConnections(0);
	polyMap->AddInputConnection(probeFilter->GetOutputPort());


	filters.clear();

	filters.push_back(probeFilter);
}

ParamVisualizer2D::ParamVisualizer2D(const std::string & file, metaData & m, int param, vtkAlgorithmOutput * probingData, vtkSmartPointer<vtkColorTransferFunction> contourColors, double range[2], int numContours) :
	ParamVisualizerBase(file, m, param),
	probeFilter(vtkProbeFilter::New()),
	contourFilter(vtkContourFilter::New()),
	polyMap(vtkPolyDataMapper::New()),
	polyAct(vtkActor::New())
{

	probeFilter->SetSourceConnection(nbs->GetOutputPort() );
	probeFilter->AddInputConnection(probingData);

	contourFilter->GenerateValues(numContours, range[0], range[1]);

	polyMap->SetScalarRange(range[0], range[1]);
	polyMap->SetLookupTable(contourColors);

	SetActiveMapper(polyMap);

	polyAct->SetMapper(polyMap);

	polyAct->GetProperty()->ShadingOff();

	SetProp(polyAct);

	ModeColorContour();
}

ParamVisualizer2D::~ParamVisualizer2D() {
	probeFilter->Delete();
	contourFilter->Delete();
	polyMap->Delete();
	polyAct->Delete();
}
