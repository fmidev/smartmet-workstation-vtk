#include "ParamVisualizerSurf.h"

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
#include <vtkTexture.h>

#include "ContourLabeler.h"

#include "VisualizerFactory.h"

#include "nbsSurface.h"



void ParamVisualizerSurf::ModeIsoLine() {


	contour->SetInputConnection(nbs->GetOutputPort());
	
	polyMap->RemoveAllInputConnections(0);

	polyMap->AddInputConnection(contour->GetOutputPort());


	filters.clear();

	filters.push_back(contour);

}

void ParamVisualizerSurf::ModeColorContour() {

	contour->RemoveAllInputConnections(0);

	polyMap->RemoveAllInputConnections(0);


	polyMap->SetInputConnection(nbs->GetOutputPort());

	filters.clear();


}

void ParamVisualizerSurf::UpdateTimeStep(double t) {
	ParamVisualizerBase::UpdateTimeStep(t);

	if (!mode) return;

	stripper->Update();

	vtkPoints *points =
		stripper->GetOutput()->GetPoints();
	vtkCellArray *cells =
		stripper->GetOutput()->GetLines();
	vtkDataArray *scalars =
		stripper->GetOutput()->GetPointData()->GetScalars();

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


		vtkIdType midPointId = indices[(numberOfPoints / 2)*(lineCount % 2)];

		double midPoint[3];
		points->GetPoint(midPointId, midPoint);

		labeler.Add(midPoint, scalars->GetTuple1(midPointId));
	}
}


vtkScalarsToColors  * ParamVisualizerSurf::getColor() {
	return polyMap->GetLookupTable();
}
double * ParamVisualizerSurf::getRange() {
	return polyMap->GetScalarRange();
}

ParamVisualizerSurf::ParamVisualizerSurf(const std::string & file, nbsMetadata & m,
	int param, vtkSmartPointer<vtkColorTransferFunction> contourColors,
	ContourLabeler &labeler, double range[2], int numContours) :

	ParamVisualizerBase(new nbsSurface(file, &m,param),m,param),
	labeler(labeler), mode(false)
{
	nbs->Update();


	contour = vtkContourFilter::New();

	contour->GenerateValues(numContours, range[0], range[1]);
	contour->SetComputeScalars(true);

	stripper = vtkStripper::New();

	stripper->SetInputConnection(contour->GetOutputPort());


	polyMap = vtkPolyDataMapper::New();

	polyMap->SetScalarRange(range);
	polyMap->SetColorModeToMapScalars();
	polyMap->SetLookupTable(contourColors);

	polyMap->SelectColorArray("Scalars");


	SetActiveMapper(polyMap);

	polyAct = vtkActor::New();

	polyAct->SetMapper(polyMap);

	//polyAct->GetProperty()->ShadingOff();

	SetProp(polyAct);

	ModeColorContour();
}

ParamVisualizerSurf::~ParamVisualizerSurf() {
	contour->Delete();
	stripper->Delete();
	polyMap->Delete();
	polyAct->Delete();
}
