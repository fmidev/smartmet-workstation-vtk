#include "ParamVisualizerSurf.h"

#include "vtkSmartPointer.h"

#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkType.h>
#include <vtkProbeFilter.h>
#include <vtkMarchingContourFilter.h>
#include <vtkStripper.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkColorTransferFunction.h>
#include <vtkTexture.h>

#include <vtkSplineFilter.h>

#include <vtkRibbonFilter.h>

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
#include "vtkCommand.h"

#include <vtkCookieCutter.h>
#include <vtkContourLoopExtraction.h>
#include <vtkTextProperty.h>

namespace fmiVis {

	class nullCommand : public vtkCommand {
	public:
		static nullCommand * New() {
			return new nullCommand{};
		}
		void Execute(vtkObject *caller, unsigned long eventId,
			void *callData) {};
	protected:
		nullCommand() {};
	};


	void ParamVisualizerSurf::ModeIsoLine() {

		polyMap->SetLookupTable(isolineColFunc);


		contour->SetInputConnection(nbs->GetOutputPort());


		append->RemoveAllInputConnections(0);




		append->SetInputConnection(0, ribbon->GetOutputPort());

		polyMap->Update();
	}

	void ParamVisualizerSurf::ModeColorContour() {

		polyMap->SetLookupTable(contourColFunc);

		contour->RemoveAllInputConnections(0);

		append->RemoveAllInputConnections(0);

		append->SetInputConnection(0, nbs->GetOutputPort());

		polyMap->Update();
	}



	void debugLines(vtkAppendPolyData* append, vtkPoints* points, vtkIdList *idList) {
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

	//hatching
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
			//append->SetInputConnection(0, contour->GetOutputPort());
			append->SetInputConnection(0, ribbon->GetOutputPort());

			contour->Update();
			stripper->Update();

			pointWidths->Reset();

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

				for (int i = 0; i < points->GetNumberOfPoints(); i++) {
					pointWidths->InsertNextTuple1(lineWidth[scalars->GetTuple1(i)]);
				}

				//if (abs(val - 5.0) < 0.01) {


					//loop->SetInputData(contour->GetOutput());

					//loop->SetScalarRange(4, 6);
					//loop->SetScalarThresholding(true);

					//loop->Update();

					//cutter->Update();

					//append->AddInputData(contour->GetOutput());
					//append->AddInputData(loop->GetOutput());

					//debugLines(append, points, idList);

				//}


			}

			//int arrId = stripper->GetOutput()->GetPointData()->AddArray(pointWidths);
			//ribbon->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "width");

		}



		if (append->GetTotalNumberOfInputConnections())
			polyMap->UpdateTimeStep(t);
	}


	vtkScalarsToColors  * ParamVisualizerSurf::getColor() {
		return polyMap->GetLookupTable();
	}
	double * ParamVisualizerSurf::getRange() {
		return polyMap->GetScalarRange();
	}

	//a hook to limit the update extent to the area currently visible on camera
	void ParamVisualizerSurf::UpdateNBS(double t)
	{
		nbs->Modified();
		if (flat)
		{
			auto areaExt = AreaUtil::FindExtentScandic(ren, meta);
			int extents[6] = { areaExt[0],areaExt[1],
								areaExt[2],areaExt[3],
								1,1 };

			nbs->UpdateTimeStep(t, -1, 1, 0, extents);
		}
		else
			nbs->UpdateTimeStep(t);

	}

	/*
	vtkMarchingCountourFilter generates isolines out of the nbs data, vtkSplineFilter smooths them and vtkRibbonFilter turns them into wide polygonal lines
	vtkAppendPolydata collects various attempts at hatching
	color contour mode hardly requires filters
	*/

	ParamVisualizerSurf::ParamVisualizerSurf(
		const std::string &file, nbsMetadata &m, ContourLabeler &labeler,
		NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac, bool flat) :

		ParamVisualizerBase(new nbsSurface(file, &m, paramIdent.GetParamIdent(), 13000, flat, true), m, paramIdent, fac),
		labeler(labeler), mode(false), flat(flat)
	{
		nbs->Update();

		//hatching
		contour = vtkMarchingContourFilter::New();
		contour->SetComputeScalars(true);
		contour->SetUseScalarTree(true);

		hatch = HatchSource::New();
		hatch->Generate(m.sizeX * 2, m.sizeY * 2, 20);


		loop = vtkContourLoopExtraction::New();
		loop->AddInputConnection(contour->GetOutputPort());
		loop->SetLoopClosureToAll();

		cutter = vtkCookieCutter::New();

		//	cutter->AddInputConnection(hatch->GetOutputPort());
		//	cutter->SetLoopsConnection(loop->GetOutputPort());

		stripper = vtkStripper::New();

		stripper->SetInputConnection(contour->GetOutputPort());

		//line smoothing
		smooth = vtkSplineFilter::New();

		smooth->SetSubdivideToLength();
		smooth->SetLength(0.1);
		smooth->SetInputConnection(stripper->GetOutputPort());

		//line width
		ribbon = vtkRibbonFilter::New();
		ribbon->SetDebug(false);
		ribbon->SetUseDefaultNormal(true);

		//consumes a warning ribbonfilter spams for unknown reasons
		auto warningSink = vtkSmartPointer<nullCommand>::New();

		ribbon->AddObserver("WarningEvent", warningSink);

		pointWidths = vtkSmartPointer<vtkFloatArray>::New();
		pointWidths->SetName("width");

		ribbon->SetInputConnection(stripper->GetOutputPort());
		ribbon->SetVaryWidth(true);
		ribbon->SetWidthFactor(8);
		ribbon->SetWidth(0.1);

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


		SetProp(polyAct);

		//called by ReloadOptions()
		//ModeColorContour();



		ReloadOptions();
	}
	void ParamVisualizerSurf::ReloadOptions()
	{

		lineWidth.clear();

		drawParam = drawParamFac->CreateDrawParam(paramIdent, nullptr);

		const int stepLimit = 20;



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

		long lower = floor(range[0] / step)*step + offset;
		long upper = ceil(range[1] / step)*step + offset;

		polyMap->SetScalarRange(lower, upper);

		if (!drawParam->UseSingleColorsWithSimpleIsoLines())
			isolineColFunc = fmiVis::makeIsolineColorFunction(drawParam.get());
		else
			isolineColFunc =
			fmiVis::constColor(&fmiVis::fmiToVtkColor(drawParam->IsolineColor()));

		contourColFunc = fmiVis::makeContourColorFunction(drawParam.get());

		auto labelProp = labeler.GetProperty();

		labelProp->SetColor(drawParam->IsolineTextColor().Red(),
			drawParam->IsolineTextColor().Green(),
			drawParam->IsolineTextColor().Blue());
		//	labelProp->SetOpacity(drawParam->IsolineTextColor().Alpha());


		labelProp->SetFrameColor(drawParam->IsolineColor().Red(),
			drawParam->IsolineColor().Green(),
			drawParam->IsolineColor().Blue());

		labelProp->SetFontSize(drawParam->SimpleIsoLineLabelHeight() * 6);


		if (drawParam->ShowSimpleIsoLineLabelBox()) {

			labelProp->SetFrame(true);

			if (!drawParam->UseTransparentFillColor()) {

				labelProp->SetBackgroundColor(drawParam->IsolineLabelBoxFillColor().Red(),
					drawParam->IsolineLabelBoxFillColor().Green(),
					drawParam->IsolineLabelBoxFillColor().Blue());

				//			labelProp->SetBackgroundOpacity(drawParam->IsolineLabelBoxFillColor().Alpha());
			}
		}
		else	labelProp->SetFrame(false);

		if (drawParam->IsoLineSplineSmoothingFactor() > 1.0f) {
			ribbon->SetInputConnection(smooth->GetOutputPort());
			smooth->SetLength(5.0f / drawParam->IsoLineSplineSmoothingFactor());
		}
		else {
			ribbon->SetInputConnection(stripper->GetOutputPort());
		}

		if (!drawParam->UseSimpleIsoLineDefinitions()) {
			std::vector<vtkColor4f> colors;
			std::vector<float> values;

			//if (drawParam->UseCustomIsoLineing()) 
			//{
			fmiVis::readCustomIsolineParams(drawParam.get(), colors, values);
			isolineColFunc = fmiVis::makeColorFunction(colors, values);
			contourColFunc = isolineColFunc;
			//}
			//else 
			//	fmiVis::readCustomContourParams(drawParam.get(), colors, values);

			contour->SetNumberOfContours(values.size());

			int i = 0;

			for (auto &val : values) {
				contour->SetValue(i++, val);
			}

			polyMap->SetScalarRange(*values.begin(), *values.rbegin());


			auto &widths = drawParam->SpecialIsoLineWidth();
			for (int i = 0; i < values.size(); i++) {
				//lineWidth.insert(std::pair(values[i], widths[(i < widths.size()) ? i : (widths.size() - 1)]) );
				lineWidth.insert(std::pair(values[i], 0.1 + double(i)*0.2));
			}
		}
		else {
			lineWidth.insert(std::pair(0, drawParam->SimpleIsoLineWidth()));
		}


		polyAct->GetProperty()->SetOpacity(drawParam->Alpha());

		decimate->SetTargetReduction(0.8);

		if (mode) ModeIsoLine();
		else  ModeColorContour();

		//polyAct->GetProperty()->ShadingOff();
		//polyAct->GetProperty()->SetOpacity(0.4);

	}

	ParamVisualizerSurf::~ParamVisualizerSurf() {
		hatch->Delete();
		cutter->Delete();
		loop->Delete();
		contour->Delete();
		stripper->Delete();
		smooth->Delete();
		append->Delete();
		polyMap->Delete();
		polyAct->Delete();
	}

}