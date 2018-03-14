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

#include "nbsSurface.h"
#include "vtkAppendPolyData.h"

#include <vtkImplicitSelectionLoop.h>
#include <vtkClipPolyData.h>
#include "HatchSource.h"
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include <vtkDecimatePolylineFilter.h>
#include <vtkContourTriangulator.h>
#include <vtkCleanPolyData.h>

#include <NFmiDrawParam.h>
#include <NFmiDataIdent.h>
#include "areaUtil.h"

void ParamVisualizerSurf::ModeIsoLine() {

	polyMap->SetLookupTable(isolineColFunc);


	contour->SetInputConnection(nbs->GetOutputPort());


	append->RemoveAllInputConnections(0);

	stripper->AddInputConnection(contour->GetOutputPort());

	
}

void ParamVisualizerSurf::ModeColorContour() {

	polyMap->SetLookupTable(contourColFunc);

	contour->RemoveAllInputConnections(0);

	append->RemoveAllInputConnections(0);
	stripper->RemoveAllInputConnections(0);

	append->SetInputConnection(0,nbs->GetOutputPort());

}



void debugLines(vtkAppendPolyData* append,vtkPoints* points, vtkIdList *idList) {
	auto data = vtkSmartPointer<vtkPolyData>::New();

	// 				loop->SetLoop(line);
	// 
	// 				clip->SetClipFunction(loop);
	// 				clip->Update();


	data->Allocate(1);

	data->SetPoints(points);

	data->InsertNextCell(VTK_POLY_LINE, idList);

	//data->ShallowCopy(clip->GetOutput());

	auto dataScalars = vtkSmartPointer<vtkFloatArray>::New();
	dataScalars->SetNumberOfComponents(1);
	for (int i = 0; i < data->GetNumberOfPoints(); ++i)
		dataScalars->InsertNextTuple1(double(i) / data->GetNumberOfPoints() * 200);

	data->GetPointData()->SetScalars(dataScalars);

	append->AddInputData(data);
}


void TriangulateContour(vtkSmartPointer<vtkPoints> line, vtkSmartPointer<vtkIdList> lineList,
	vtkSmartPointer<vtkDecimatePolylineFilter> decimate, vtkSmartPointer<vtkContourTriangulator> triangulate, vtkSmartPointer<vtkAppendPolyData> append)
{

	auto data = vtkSmartPointer<vtkPolyData>::New();

	data->Allocate(1);

	data->SetPoints(line);

	data->InsertNextCell(VTK_POLY_LINE, lineList);


	decimate->SetInputData(data);
	decimate->Update();

	triangulate->SetInputData(decimate->GetOutput());

	triangulate->Update();

	data->ShallowCopy(triangulate->GetOutput());
	append->AddInputData(data);
}

void ParamVisualizerSurf::UpdateTimeStep(double t) {
	UpdateNBS(t);

	if (mode) {

		append->RemoveAllInputs();
		append->SetInputConnection(0, contour->GetOutputPort());

		contour->Update();
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
			/*
			if (abs(val - 60) < 0.01) {


				auto line = vtkSmartPointer<vtkPoints>::New();
				auto idList = vtkSmartPointer<vtkIdList>::New();

				for (int i = 0; i < numberOfPoints; i++)
					idList->InsertNextId(indices[i]);
				points->GetPoints(idList, line);

				auto lineList = vtkSmartPointer<vtkIdList>::New();
				for (int i = 0; i < numberOfPoints; i++)
					lineList->InsertNextId(i);
				lineList->InsertNextId(0);


				TriangulateContour(line, lineList, decimate, triangulate, append);
 
				debugLines(append, points, idList);

			}
			*/

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

void ParamVisualizerSurf::UpdateNBS(double t)
{
	nbs->Modified();
	if (flat)
	{
		auto areaExt = AreaUtil::FindExtentScandic(ren, meta.dataInfo->Area());
		int extents[6] = {	areaExt[0],areaExt[1],
							areaExt[2],areaExt[3],
							1,1 };

		nbs->UpdateTimeStep(t, -1, 1, 0, extents);
	}
	else
		nbs->UpdateTimeStep(t);

}

ParamVisualizerSurf::ParamVisualizerSurf(
	const std::string &file, nbsMetadata &m, ContourLabeler &labeler,
	NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac, bool flat) :

	ParamVisualizerBase(new nbsSurface(file, &m, paramIdent.GetParamIdent(), 13000, flat, true), m, paramIdent, fac),
	labeler(labeler), mode(false), flat(flat)
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
	contour->SetComputeScalars(true);

	stripper = vtkStripper::New();

	stripper->SetInputConnection(contour->GetOutputPort());

	append = vtkAppendPolyData::New();

	polyMap = vtkPolyDataMapper::New();

	polyMap->AddInputConnection(append->GetOutputPort());

	polyMap->SelectColorArray("Scalars");

	polyMap->SetColorModeToMapScalars();

	triangulate = vtkSmartPointer<vtkContourTriangulator>::New();

	clean = vtkSmartPointer<vtkCleanPolyData>::New();
	decimate = vtkSmartPointer<vtkDecimatePolylineFilter>::New();

	//decimate->AddInputConnection(clean->GetOutputPort());

	SetActiveMapper(polyMap);

	polyAct = vtkActor::New();

	polyAct->SetMapper(polyMap);

	ReloadOptions();

	SetProp(polyAct);

	ModeColorContour();
}

void ParamVisualizerSurf::ReloadOptions()
{

	auto drawParam = drawParamFac->CreateDrawParam(paramIdent, nullptr);

	const int stepLimit = 100;


	
	auto range = meta.getParamRange(param);

	auto step = drawParam->IsoLineGab();

	int stepCount = (range[1] - range[0]) / step;
	if (stepCount > stepLimit) stepCount = stepLimit;

/*	contour->GenerateValues(stepCount,
		drawParam->SimpleIsoLineZeroValue() - step * stepCount/2,
		drawParam->SimpleIsoLineZeroValue() + step * stepCount/2);
*/
	contour->GenerateValues(stepCount, range[0], range[1]);
	//stripper->SetJoinContiguousSegments(1);


	double offset = drawParam->SimpleIsoLineZeroValue();

	long lower = floor(range[0] / step )*step + offset;
	long upper = ceil(range[1] / step )*step + offset;

	polyMap->SetScalarRange(lower,upper);

	if (!drawParam->UseSingleColorsWithSimpleIsoLines())
		isolineColFunc = fmiVis::makeIsolineColorFunction(drawParam.get());
	else
		isolineColFunc =
			fmiVis::constColor( &fmiVis::fmiToVtkColor(drawParam->IsolineColor()) );

	contourColFunc =  fmiVis::makeContourColorFunction(drawParam.get());

	if (drawParam->UseCustomIsoLineing() || drawParam->UseCustomColorContouring()) {
		std::vector<vtkColor4f> colors;
		std::vector<float> values;

		if (drawParam->UseCustomIsoLineing()) 
		{
			isolineColFunc = fmiVis::makeColorFunction(colors, values);
			fmiVis::readCustomIsolineParams(drawParam.get(), colors, values);
		}
		else 
			fmiVis::readCustomContourParams(drawParam.get(), colors, values);

		contour->SetNumberOfContours(values.size());

		int i = 0;

		for (auto &val : values) {
			contour->SetValue(i++, val);
		}

		polyMap->SetScalarRange(*values.begin(), *values.rbegin());
	}

	decimate->SetTargetReduction(0.9);

	//polyAct->GetProperty()->ShadingOff();
	//polyAct->GetProperty()->SetOpacity(0.4);


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
