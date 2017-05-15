#include "ParamVisualizerWindVec.h"

#include "nbsWindVectors.h"

#include <vtkPolyDataMapper.h>

#include <vtkGlyph3D.h>

#include <vtkStreamTracer.h>
#include <vtkTubeFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkActor.h>

#include "Windbarb.h"

#include "VisualizerFactory.h"
#include "vtkColorTransferFunction.h"
#include "vtkScalarsToColors.h"

void ParamVisualizerWindVec::ModeStreamline() {

	nbs->setSubSample(3);

	cleaner->SetInputConnection(seedData);
	streamer->SetInputConnection(nbs->GetOutputPort());

	glypher->RemoveAllInputConnections(0);

	map->RemoveAllInputConnections(0);

	map->AddInputConnection(tuber->GetOutputPort());

	filters.clear();

	filters.push_back(cleaner);
	filters.push_back(streamer);
	filters.push_back(tuber);

	act->SetPosition(0, 0, 0);
}
void ParamVisualizerWindVec::ModeGlyph() {


	nbs->setSubSample(14);


	glypher->SetInputConnection(nbs->GetOutputPort());


	cleaner->RemoveAllInputConnections(0);
	streamer->RemoveAllInputConnections(0);

	map->RemoveAllInputConnections(0);

	map->AddInputConnection(glypher->GetOutputPort());

	filters.clear();

	filters.push_back(glypher);

	act->SetPosition(10, 10, 10);
}

ParamVisualizerWindVec::ParamVisualizerWindVec(const std::string &file, nbsMetadata &m, vtkAlgorithmOutput* seedData) :
	ParamVisualizerBase(new nbsWindVectors(file, &m)), seedData(seedData), mode(false)
{


	nbs->Update();



	glypher = vtkGlyph3D::New();
	GenerateBarbs(glypher);
	glypher->OrientOn();
	glypher->SetIndexModeToVector();
	glypher->SetVectorModeToUseVector();
	glypher->SetScaleModeToDataScalingOff();
	glypher->SetColorModeToColorByScalar();


	cleaner = vtkCleanPolyData::New();

	cleaner->SetAbsoluteTolerance(25);
	cleaner->SetToleranceIsAbsolute(true);

	streamer = vtkStreamTracer::New();

	streamer->SetSourceConnection(cleaner->GetOutputPort());
	streamer->SetMaximumPropagation(400);
	streamer->SetIntegrationDirectionToBoth();
	streamer->SetInitialIntegrationStep(0.4);
	streamer->SetIntegratorTypeToRungeKutta4();

	tuber = vtkTubeFilter::New();
	tuber->SetInputConnection(streamer->GetOutputPort());
	tuber->SetInputArrayToProcess(1, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "ImageScalars");
	tuber->SetVaryRadiusToVaryRadiusByVector();
	tuber->SetRadius(1);
	tuber->SetRadiusFactor(4);

	tuber->SetNumberOfSides(6);


	map = vtkPolyDataMapper::New();
	map->SetInputConnection(tuber->GetOutputPort());
	map->SetScalarRange(0,70);
	map->SetColorModeToMapScalars();
	map->SetLookupTable(VisualizerFactory::blueToRedColor(0, 70) );

	act = vtkActor::New();
	act->SetMapper(map);

	SetActiveMapper(map);
	SetProp(act);

	ModeStreamline();
}

ParamVisualizerWindVec::~ParamVisualizerWindVec() {
	glypher->Delete();
	cleaner->Delete();
	streamer->Delete();
	tuber->Delete();
	map->Delete();
	act->Delete();
}