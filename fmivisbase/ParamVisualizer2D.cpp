#include "ParamVisualizer2D.h"

#include "vtkSmartPointer.h"

#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkType.h>
#include <vtkProbeFilter.h>
#include <vtkContourFilter.h>
#include <vtkStripper.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkColorTransferFunction.h>

#include "newBaseSourcer.h"

#include "ContourLabeler.h"



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

void ParamVisualizer2D::UpdateTimeStep(double t) {
	ParamVisualizerBase::UpdateTimeStep(t);

	if (!mode) return;

	contourStripper->Update();

	vtkPoints *points =
		contourStripper->GetOutput()->GetPoints();
	vtkCellArray *cells =
		contourStripper->GetOutput()->GetLines();
	vtkDataArray *scalars =
		contourStripper->GetOutput()->GetPointData()->GetScalars();

	vtkIdType *indices;
	vtkIdType numberOfPoints;
	unsigned int lineCount = 0;
	for (cells->InitTraversal();
		cells->GetNextCell(numberOfPoints, indices);
		lineCount++)
	{
		if (numberOfPoints < 10)
		{
			continue;
		}


		vtkIdType midPointId = indices[(numberOfPoints / 2)*(lineCount%2)];

		double midPoint[3];
		points->GetPoint(midPointId, midPoint);

		labeler.Add(midPoint, scalars->GetTuple1(midPointId));
	}
}

ParamVisualizer2D::ParamVisualizer2D(const std::string & file, nbsMetadata & m, 
	int param, vtkAlgorithmOutput * probingData,
	vtkSmartPointer<vtkColorTransferFunction> contourColors,
	ContourLabeler &labeler, double range[2], int numContours) :

	ParamVisualizerBase(file, m, param),
	labeler(labeler),


	mode(false)
{

	probeFilter = vtkSmartPointer<vtkProbeFilter>::New();

	probeFilter->SetSourceConnection(nbs->GetOutputPort() );
	probeFilter->AddInputConnection(probingData);

	contourFilter = vtkSmartPointer<vtkContourFilter>::New();

	contourFilter->GenerateValues(numContours, range[0], range[1]);

	contourStripper = vtkSmartPointer<vtkStripper>::New();

	contourStripper->SetInputConnection(contourFilter->GetOutputPort());

	polyMap = vtkSmartPointer<vtkPolyDataMapper>::New();

	polyMap->SetLookupTable(contourColors);
	polyMap->SetScalarRange(range[0], range[1]);

	SetActiveMapper(polyMap);

	polyAct = vtkSmartPointer<vtkActor>::New();

	polyAct->SetMapper(polyMap);

	polyAct->GetProperty()->ShadingOff();

	SetProp(polyAct);

	ModeColorContour();
}


vtkScalarsToColors  * ParamVisualizer2D::getColor() {
	return polyMap->GetLookupTable();
}
double * ParamVisualizer2D::getRange() {
	return polyMap->GetScalarRange();
}