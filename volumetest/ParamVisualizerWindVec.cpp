#include "ParamVisualizerWindVec.h"

#include "nbsWindVectors.h"

#include <vtkPolyDataMapper.h>
#include <vtkAlgorithmOutput.h>

#include <vtkProbeFilter.h>
#include <vtkAssignAttribute.h>
#include <vtkPointData.h>

#include <vtkGlyph3D.h>

#include <vtkStreamTracer.h>
#include <vtkTubeFilter.h>
#include <vtkMaskPoints.h>
#include <vtkActor.h>

#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelectedIds.h>

#include "Windbarb.h"

#include "VisualizerFactory.h"
#include "vtkColorTransferFunction.h"
#include "vtkScalarsToColors.h"

void ParamVisualizerWindVec::ModeStreamline() {
	
	streamer->SetInputConnection(nbs->GetOutputPort());

	glypher->RemoveAllInputConnections(0);

	map->RemoveAllInputConnections(0);

	map->AddInputConnection(tuber->GetOutputPort());

	filters.clear();
	
	filters.push_back(extract);
	filters.push_back(streamer);
	filters.push_back(tuber);
}
void ParamVisualizerWindVec::ModeGlyph() {


	glypher->SetInputConnection(assign->GetOutputPort());

	streamer->RemoveAllInputConnections(0);

	map->RemoveAllInputConnections(0);

	map->AddInputConnection(glypher->GetOutputPort());

	filters.clear();

	filters.push_back(extract);
	filters.push_back(probe);
	filters.push_back(assign);
	filters.push_back(glypher);
}


ParamVisualizerWindVec::ParamVisualizerWindVec(const std::string &file, nbsMetadata &m, vtkAlgorithmOutput* seedData) :
	ParamVisualizerBase(new nbsWindVectors(file, &m)), seedData(seedData), mode(false)
{

	nbs->setSubSample(3);
	nbs->Update();

	auto ids = vtkSmartPointer<vtkIdTypeArray>::New();
	ids->SetNumberOfComponents(1);

	auto width = 80;

	auto downScale = 10;

	// Set values
	for (unsigned int ix = 0; ix < width/downScale; ++ix)
		for (unsigned int iy = 0; iy < width / downScale; ++iy)
	{
		ids->InsertNextValue(ix*downScale + width*iy*downScale);
	}

	auto selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
	selectionNode->SetFieldType(vtkSelectionNode::POINT);
	selectionNode->SetContentType(vtkSelectionNode::INDICES);
	selectionNode->SetSelectionList(ids);

	auto selection = vtkSmartPointer<vtkSelection>::New();
	selection->AddNode(selectionNode);

	extract = vtkExtractSelectedIds::New();
	extract->SetInputConnection(0, seedData );

	extract->SetInputData(1, selection);

	probe = vtkProbeFilter::New();
	probe->SetPassPointArrays(true);
	probe->SetInputConnection(extract->GetOutputPort());
	probe->SetSourceConnection(nbs->GetOutputPort());

	assign = vtkAssignAttribute::New();

	assign->SetInputConnection(probe->GetOutputPort());

	assign->Assign("ImageScalars", vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA);

	assign->Update();

	assign->GetOutput()->PrintSelf(cout, vtkIndent());

	glypher = vtkGlyph3D::New();

	//glyph size
	GenerateBarbs(glypher, 10);


	glypher->OrientOn();
	glypher->SetIndexModeToVector();
	glypher->SetVectorModeToUseVector();
	glypher->SetScaleModeToDataScalingOff();
	glypher->SetColorModeToColorByScalar();


	streamer = vtkStreamTracer::New();

	streamer->SetSourceConnection(extract->GetOutputPort());
	streamer->SetMaximumPropagation(400);
	streamer->SetIntegrationDirectionToBoth();

	//integration accuracy for streamlines
	streamer->SetInitialIntegrationStep(0.5);
	streamer->SetIntegratorTypeToRungeKutta4();

	tuber = vtkTubeFilter::New();
	tuber->SetInputConnection(streamer->GetOutputPort());
	tuber->SetInputArrayToProcess(1, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "ImageScalars");
	tuber->SetVaryRadiusToVaryRadiusByVector();

	//streamline width
	tuber->SetRadius(0.3);
	tuber->SetRadiusFactor(6);

	tuber->SetNumberOfSides(4);


	map = vtkPolyDataMapper::New();
	map->SetInputConnection(tuber->GetOutputPort());


	//colors
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
	probe->Delete();
	glypher->Delete();
	extract->Delete();
	streamer->Delete();
	tuber->Delete();
	map->Delete();
	act->Delete();
}