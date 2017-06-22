#include "ParamVisualizerWindVec2D.h"


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
#include <vtkRibbonFilter.h>
#include <vtkTubeFilter.h>
#include <vtkMaskPoints.h>
#include <vtkActor.h>

#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelectedIds.h>

#include "Windbarb.h"

#include "VisualizerFactory.h"
#include "nbsSurfaceWind.h"

#include <vtkPNGReader.h>


void ParamVisualizerWindVec2D::ModeStreamline() {

	streamer->AddInputConnection(nbs->GetOutputPort());

	glypher->RemoveAllInputConnections(0);

	map->RemoveAllInputConnections(0);

	map->AddInputConnection(ribbon->GetOutputPort());

	static vtkSmartPointer<vtkTexture> t;
	if (!t) {
		auto mapReader = vtkSmartPointer<vtkPNGReader>::New();
		mapReader->SetFileName("stream.png");

		mapReader->Update();

		t = vtkSmartPointer<vtkTexture>::New();
		t->SetInputData(mapReader->GetOutput());
		t->SetInterpolate(false);
		t->Update();

	}

	act->SetTexture(t);

	filters.clear();

	filters.push_back(extract);
	filters.push_back(streamer);
	filters.push_back(ribbon);
}
void ParamVisualizerWindVec2D::ModeGlyph() {


	glypher->AddInputConnection(assign->GetOutputPort());

	streamer->RemoveAllInputConnections(0);

	map->RemoveAllInputConnections(0);

	map->AddInputConnection(glypher->GetOutputPort());

	act->SetTexture(nullptr);

	filters.clear();

	filters.push_back(extract);
	filters.push_back(assign);
	filters.push_back(glypher);
}


ParamVisualizerWindVec2D::ParamVisualizerWindVec2D(const std::string &file, nbsMetadata &m) :
	ParamVisualizerBase(new nbsSurfaceWind(file, &m), m, param), mode(false)
{

	//nbs->setSubSample(3);
	nbs->Update();

	auto ids = vtkSmartPointer<vtkIdTypeArray>::New();
	ids->SetNumberOfComponents(1);

	auto width = 100;

	auto downScale = 10;


	// Set values
	for (unsigned int ix = 0; ix < width / downScale; ++ix)
		for (unsigned int iy = 0; iy < width / downScale; ++iy)
		{
			ids->InsertNextValue(width / downScale + ix*downScale + (width / downScale / 2 + iy*downScale)*width);
		}

	auto selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
	selectionNode->SetFieldType(vtkSelectionNode::POINT);
	selectionNode->SetContentType(vtkSelectionNode::INDICES);
	selectionNode->SetSelectionList(ids);

	auto selection = vtkSmartPointer<vtkSelection>::New();
	selection->AddNode(selectionNode);

	extract = vtkExtractSelectedIds::New();
	extract->SetInputConnection(0, nbs->GetOutputPort());

	extract->SetInputData(1, selection);

	assign = vtkAssignAttribute::New();

	assign->SetInputConnection(extract->GetOutputPort());

	assign->Assign("Scalars", vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA);

	assign->Update();


	auto transform = vtkSmartPointer<vtkTransform>::New();


	//transform->Scale(0.2, 0.2, 0.2);
	transform->Translate(-7.5, 0, 0);
	transform->RotateX(90);
	transform->Update();

	glypher = vtkGlyph3D::New();

	//glyph size
	GenerateBarbs(glypher, 10);


	glypher->OrientOn();
	glypher->SetIndexModeToVector();
	glypher->SetVectorModeToUseVector();
	glypher->SetScaleModeToDataScalingOff();
	glypher->SetColorModeToColorByScalar();
	glypher->SetSourceTransform(transform);

	streamer = vtkStreamTracer::New();

	streamer->SetSourceConnection(assign->GetOutputPort());
 	streamer->SetMaximumPropagation(300);
 	//streamer->SetIntegrationDirectionToBoth();
	streamer->SurfaceStreamlinesOn();
	streamer->SetTerminalSpeed(0.025);

 	//integration accuracy for streamlines
 	streamer->SetInitialIntegrationStep(0.5);
 	streamer->SetIntegratorTypeToRungeKutta4();



	ribbon = vtkRibbonFilter::New(); //causes a warning apparently
	ribbon->SetInputConnection(streamer->GetOutputPort());

	ribbon->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "Scalars");
	ribbon->SetUseDefaultNormal(true);
	ribbon->SetVaryWidth(true);
	ribbon->SetWidthFactor(2.5);
	ribbon->SetGenerateTCoordsToNormalizedLength () ;
	ribbon->SetWidth(.3);

	map = vtkPolyDataMapper::New();
	//colors
 	map->SetScalarRange(0, 150);
 	map->SetColorModeToMapScalars();
 	map->SetLookupTable(VisualizerFactory::blueToRedColor(0, 150));

	act = vtkActor::New();
	act->SetMapper(map);



	SetActiveMapper(map);
	SetProp(act);

	ModeStreamline();
}

ParamVisualizerWindVec2D::~ParamVisualizerWindVec2D() {
	glypher->Delete();
	extract->Delete();
	streamer->Delete();
	ribbon->Delete();
	map->Delete();
	act->Delete();
}

