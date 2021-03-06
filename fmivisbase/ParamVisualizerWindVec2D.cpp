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
#include <vtkEvenlySpacedStreamlines2D.h>
#include <vtkRibbonFilter.h>
#include <vtkTubeFilter.h>
#include <vtkMaskPoints.h>
#include <vtkActor.h>
#include <vtkProperty.h>

#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelectedIds.h>

#include "Glyphs.h"

#include "nbsSurfaceWind.h"

#include <vtkPNGReader.h>


#include <NFmiDataIdent.h>

#include "DrawOptions.h"

namespace fmiVis {

	void ParamVisualizerWindVec2D::ModeStreamline() {

		mode = mode_stream;

		spacedStreamer->SetInputConnection(nbs->GetOutputPort());

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
		filters.push_back(spacedStreamer);
		filters.push_back(ribbon);
	}
	void ParamVisualizerWindVec2D::ModeBarb() {

		mode = mode_barb;

		glypher->RemoveAllInputConnections(0);
		glypher->AddInputConnection(assign->GetOutputPort());
		glypher->SetIndexModeToVector();
		SetSourceBarb(glypher, 10);

		streamer->RemoveAllInputConnections(0);

		map->RemoveAllInputConnections(0);

		map->AddInputConnection(glypher->GetOutputPort());


		map->SelectColorArray(nullptr);

		act->SetTexture(nullptr);

		filters.clear();

		filters.push_back(extract);
		filters.push_back(assign);
		filters.push_back(glypher);
	}

	void ParamVisualizerWindVec2D::ModeArrow() {

		mode = mode_arrow;

		glypher->RemoveAllInputConnections(0);
		glypher->AddInputConnection(assign->GetOutputPort());
		glypher->SetIndexModeToOff();
		SetSourceArrow(glypher, 10);

		streamer->RemoveAllInputConnections(0);

		map->RemoveAllInputConnections(0);

		map->AddInputConnection(glypher->GetOutputPort());

		act->SetTexture(nullptr);

		filters.clear();

		filters.push_back(extract);
		filters.push_back(assign);
		filters.push_back(glypher);
	}

	//ribbonfilter emits a warning for no discernible reason
	//TODO use a null handler as a sink instead, like in surface visualizer
	void ParamVisualizerWindVec2D::UpdateTimeStep(double t)
	{

		vtkObject::GlobalWarningDisplayOff();

		ParamVisualizerBase::UpdateTimeStep(t);
		//if (mode == mode_stream) {
		//	auto arr = streamer->GetOutput()->GetPointData()->GetScalars();
		//	cout << arr->GetRange()[0] << " : " << arr->GetRange()[1] << '\n';
		//}

		vtkObject::GlobalWarningDisplayOn();


	}


	//TODO use nbs subsampling instead
	ParamVisualizerWindVec2D::ParamVisualizerWindVec2D(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac) :
		ParamVisualizerBase(new nbsSurfaceWind(file, &m), m, paramIdent, fac), mode(mode_stream)
	{

		//nbs->setSubSample(3);
		nbs->Update();

		auto ids = vtkSmartPointer<vtkIdTypeArray>::New();
		ids->SetNumberOfComponents(1);

		auto width = 100;

		auto downScale = 15;



		//Subsampling the grid manually instead of using nbs subsampling??
		for (unsigned int iy = 0; iy < width / downScale; ++iy)
			for (unsigned int ix = 0; ix < width / downScale; ++ix)
			{
				ids->InsertNextValue(width / downScale + iy * downScale + (width / downScale / 2 + ix * downScale)*width);
			}

		auto selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
		selectionNode->SetFieldType(vtkSelectionNode::POINT);
		selectionNode->SetContentType(vtkSelectionNode::INDICES);
		selectionNode->SetSelectionList(ids);

		auto selection = vtkSmartPointer<vtkSelection>::New();
		selection->AddNode(selectionNode);

		extract = vtkSmartPointer<vtkExtractSelectedIds>::New();
		extract->SetInputConnection(0, nbs->GetOutputPort());

		extract->SetInputData(1, selection);

		assign = vtkSmartPointer<vtkAssignAttribute>::New();

		assign->SetInputConnection(extract->GetOutputPort());

		assign->Assign("Scalars", vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA);

		assign->Update();


		auto transform = vtkSmartPointer<vtkTransform>::New();


		//transform->Scale(0.2, 0.2, 0.2);
		transform->Translate(-7.5, 0, 0);
		transform->RotateX(90);
		transform->Update();

		glypher = vtkSmartPointer<vtkGlyph3D>::New();


		glypher->OrientOn();

		glypher->SetVectorModeToUseVector();
		glypher->SetScaleModeToDataScalingOff();
		glypher->SetColorModeToColorByScalar();
		glypher->SetSourceTransform(transform);

		streamer = vtkSmartPointer<vtkStreamTracer>::New();

		//streamer->SetSourceConnection(assign->GetOutputPort());
		streamer->SetMaximumPropagation(300);
		//streamer->SetIntegrationDirectionToBoth();
		streamer->SurfaceStreamlinesOn();
		streamer->SetTerminalSpeed(0.025);

		//integration accuracy for streamlines
		streamer->SetInitialIntegrationStep(2);
		streamer->SetIntegratorTypeToRungeKutta4();

		streamer->SetComputeVorticity(false);

		spacedStreamer = vtkSmartPointer<vtkEvenlySpacedStreamlines2D>::New();

		spacedStreamer->SetSeparatingDistance(4);
		spacedStreamer->SetSeparatingDistanceRatio(0.005);
		spacedStreamer->SetStartPosition(50, 50, 0.1);
		spacedStreamer->SetMaximumNumberOfSteps(2000);
		spacedStreamer->SetTerminalSpeed(0.025);

		spacedStreamer->SetClosedLoopMaximumDistance(0.2);
		spacedStreamer->SetInitialIntegrationStep(2);
		spacedStreamer->SetIntegratorTypeToRungeKutta4();

		ribbon = vtkSmartPointer<vtkRibbonFilter>::New(); //causes a warning apparently
		ribbon->SetInputConnection(spacedStreamer->GetOutputPort());

		//ribbon->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "Scalars");

		ribbon->UseDefaultNormalOn();
		ribbon->SetDefaultNormal(0, 0, 1);

		ribbon->SetVaryWidth(false);
		ribbon->SetWidthFactor(5);
		ribbon->SetGenerateTCoordsToNormalizedLength();
		ribbon->SetWidth(0.5);

		map = vtkSmartPointer<vtkPolyDataMapper>::New();
		//colors
		map->SetScalarRange(0, 4);
		map->SetColorModeToMapScalars();
		map->SetLookupTable(fmiVis::blueToRedColFunc(0, 4));
		map->GetLookupTable()->SetVectorModeToMagnitude();


		act = vtkSmartPointer<vtkActor>::New();
		act->SetMapper(map);

		//flattens the glyph shading, making it look 2D
		act->GetProperty()->LightingOff();

		SetActiveMapper(map);
		SetProp(act);

		ModeStreamline();
	}

}