#include "ParamVisualizerWindVec.h"

#include "nbsWindVectors.h"

#include <vtkPolyDataMapper.h>
#include <vtkAlgorithmOutput.h>

#include <vtkProbeFilter.h>
#include <vtkAssignAttribute.h>
#include <vtkPointData.h>

#include <vtkColorTransferFunction.h>
#include <vtkScalarsToColors.h>
#include <vtkTransform.h>
#include <vtkGlyph3D.h>

#include <vtkStreamTracer.h>
#include <vtkTubeFilter.h>
#include <vtkMaskPoints.h>
#include <vtkActor.h>

#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelectedIds.h>

#include <NFmiDataIdent.h>

#include "DrawOptions.h"

#include "Glyphs.h"


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


ParamVisualizerWindVec::ParamVisualizerWindVec(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac, vtkAlgorithmOutput* seedData) :
	ParamVisualizerBase(new nbsWindVectors(file, &m),m, paramIdent,fac), seedData(seedData), mode(false)
{

	//nbs->setSubSample(3);
	nbs->Update();

	auto ids = vtkSmartPointer<vtkIdTypeArray>::New();
	ids->SetNumberOfComponents(1);

	auto width = 80;

	auto downScale = 10;


	// Set values
	for (unsigned int ix = 0; ix < width/downScale; ++ix)
		for (unsigned int iy = 0; iy < width / downScale; ++iy)
	{
		ids->InsertNextValue(width / downScale + ix*downScale + (width / downScale / 2 +  iy*downScale)*width);
	}

	auto selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
	selectionNode->SetFieldType(vtkSelectionNode::POINT);
	selectionNode->SetContentType(vtkSelectionNode::INDICES);
	selectionNode->SetSelectionList(ids);

	auto selection = vtkSmartPointer<vtkSelection>::New();
	selection->AddNode(selectionNode);

	extract = vtkSmartPointer<vtkExtractSelectedIds>::New();
	extract->SetInputConnection(0, seedData );

	extract->SetInputData(1, selection);

	probe = vtkSmartPointer<vtkProbeFilter>::New();
	probe->SetPassPointArrays(true);
	probe->SetInputConnection(extract->GetOutputPort());
	probe->SetSourceConnection(nbs->GetOutputPort());

	assign = vtkSmartPointer<vtkAssignAttribute>::New();

	assign->SetInputConnection(probe->GetOutputPort());

	assign->Assign("ImageScalars", vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA);

	assign->Update();

	auto transform = vtkSmartPointer<vtkTransform>::New();

	transform->Translate(-7.5, 0, 0);
	transform->Update();

	glypher = vtkSmartPointer<vtkGlyph3D>::New();

	//glyph size
	SetSourceBarb(glypher, 10);


	glypher->OrientOn();
	glypher->SetIndexModeToVector();
	glypher->SetVectorModeToUseVector();
	glypher->SetScaleModeToDataScalingOff();
	glypher->SetColorModeToColorByScalar();
	glypher->SetSourceTransform(transform);

	streamer = vtkSmartPointer<vtkStreamTracer>::New();

	streamer->SetSourceConnection(extract->GetOutputPort());
	streamer->SetMaximumPropagation(400);
	streamer->SetIntegrationDirectionToBoth();

	//integration accuracy for streamlines
	streamer->SetInitialIntegrationStep(0.5);
	streamer->SetIntegratorTypeToRungeKutta4();

	tuber = vtkSmartPointer<vtkTubeFilter>::New();
	tuber->SetInputConnection(streamer->GetOutputPort());
	tuber->SetInputArrayToProcess(1, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "ImageScalars");
	tuber->SetVaryRadiusToVaryRadiusByVector();

	//streamline width
	tuber->SetRadius(1);
	tuber->SetRadiusFactor(4);

	tuber->SetNumberOfSides(4);


	map = vtkSmartPointer<vtkPolyDataMapper>::New();
	map->SetInputConnection(tuber->GetOutputPort());


	//colors
	map->SetScalarRange(0,150);
	map->SetColorModeToMapScalars();
	map->SetLookupTable(fmiVis::blueToRedColFunc(0, 150) );

	act = vtkSmartPointer<vtkActor>::New();
	act->SetMapper(map);

	SetActiveMapper(map);
	SetProp(act);

	ModeStreamline();
}