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
#include "vtkAppendPolyData.h"

#include <vtkImplicitSelectionLoop.h>
#include <vtkClipPolyData.h>
#include "HatchSource.h"
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

void ParamVisualizerSurf::ModeIsoLine() {


	contour->SetInputConnection(nbs->GetOutputPort());
	
	append->RemoveAllInputConnections(0);

	stripper->AddInputConnection(contour->GetOutputPort());
	append->SetInputConnection(0,contour->GetOutputPort());

}

void ParamVisualizerSurf::ModeColorContour() {

	contour->RemoveAllInputConnections(0);

	append->RemoveAllInputConnections(0);
	stripper->RemoveAllInputConnections(0);

	append->SetInputConnection(0,nbs->GetOutputPort());

}

void ParamVisualizerSurf::UpdateTimeStep(double t) {
	nbs->UpdateTimeStep(t);

	if (mode) {


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

			vtkIdType midPointId = indices[(numberOfPoints / 2)*(lineCount % 2)];

			auto val = scalars->GetTuple1(midPointId);

			if (numberOfPoints > 10)
			{

				double midPoint[3];
				points->GetPoint(midPointId, midPoint);

				labeler.Add(midPoint, val);

			}
		
			/*if (abs(val - 20) < 0.01) {

				auto line = vtkSmartPointer<vtkPoints>::New();
				auto idList = vtkSmartPointer<vtkIdList>::New();

				for (int i = 0; i < numberOfPoints; i++)
					idList->InsertNextId(indices[i]);
				points->GetPoints(idList, line);

				loop->SetLoop(line);

				clip->SetClipFunction(loop);
				clip->Update();

				auto data = vtkSmartPointer<vtkPolyData>::New();

				data->Allocate(1);

				data->SetPoints(points);

				data->InsertNextCell(VTK_POLY_LINE,idList);

 				//data->ShallowCopy(clip->GetOutput());

				auto dataScalars = vtkSmartPointer<vtkFloatArray>::New();
				dataScalars->SetNumberOfComponents(1);
				for (int i = 0; i < data->GetNumberOfPoints(); ++i)
					dataScalars->InsertNextTuple1(double(i)/data->GetNumberOfPoints()*200);

				data->GetPointData()->SetScalars(dataScalars);

				append->AddInputData(data);
			}*/


		} 

	}

	if(append->GetTotalNumberOfInputConnections())
		polyMap->UpdateTimeStep(t);
}


vtkScalarsToColors  * ParamVisualizerSurf::getColor() {
	return polyMap->GetLookupTable();
}
double * ParamVisualizerSurf::getRange() {
	return polyMap->GetScalarRange();
}

ParamVisualizerSurf::ParamVisualizerSurf(const std::string & file, nbsMetadata & m,
	int param, vtkSmartPointer<vtkColorTransferFunction> contourColors,
	ContourLabeler &labeler, double range[2], int numContours, bool flat) :

	ParamVisualizerBase(new nbsSurface(file, &m,param,13000,flat),m,param),
	labeler(labeler), mode(false)
{
	nbs->Update();

	hatch = HatchSource::New();
	hatch->Generate(m.sizeX * 2, m.sizeY * 2, 20);

	loop = vtkImplicitSelectionLoop::New();

	loop->AutomaticNormalGenerationOff();
	loop->SetNormal(0, 0, 1);

	clip = vtkClipPolyData::New();

	//clip->InsideOutOn();
	//clip->GenerateClippedOutputOn();

	clip->AddInputConnection(hatch->GetOutputPort());

	contour = vtkContourFilter::New();

	contour->GenerateValues(numContours, range[0], range[1]);
	contour->SetComputeScalars(true);

	stripper = vtkStripper::New();

	//stripper->SetJoinContiguousSegments(1);

	stripper->SetInputConnection(contour->GetOutputPort());

	append = vtkAppendPolyData::New();

	polyMap = vtkPolyDataMapper::New();

	polyMap->AddInputConnection(append->GetOutputPort());

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
	hatch->Delete();
	loop->Delete();
	clip->Delete();
	contour->Delete();
	stripper->Delete();
	append->Delete();
	polyMap->Delete();
	polyAct->Delete();
}
