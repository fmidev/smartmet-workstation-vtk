#include "ViewportFactory.h"

#include <vector>
#include <memory>

#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkLight.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include "VisualizerManager.h"


#include "ParamVisualizerWindVec2D.h"
#include "ParamVisualizerSurf.h"
#include "ParamVisualizerIcon.h"
#include "ParamVisualizerArrayText.h"
#include "ParamVisualizerPolyText.h"
#include "ParamVisualizerText.h"

#include "ViewportManager.h"
#include "ViewportManagerTimeGrid.h"
#include "vtkPNGReader.h"
#include "vtkTexture.h"
#include "nbsSurface.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkSliderWidget.h"
#include "vtkSliderRepresentation2D.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "TimeAnimator.h"

#include "Interactor.h"

#include "CameraCallback.h"
#include <NFmiDrawParamFactory.h>
#include "DrawOptions.h"

using namespace std::string_literals;
using namespace fmiVis;


std::map<int, std::string> ViewportFactory::paramsSurf = {
	std::pair<int,std::string>(kFmiPressure,"kFmiPressure"),
	std::pair<int,std::string>(kFmiTemperature,"kFmiTemperature"),
	std::pair<int,std::string>(kFmiHumidity,"kFmiHumidity"),
	std::pair<int,std::string>(kFmiWindSpeedMS,"kFmiWindSpeedMS"),
	std::pair<int,std::string>(kFmiTotalCloudCover,"kFmiTotalCloudCover"),
	std::pair<int,std::string>(kFmiPrecipitation1h,"kFmiPrecipitation1h"),
};

visID fmiVis::addVis(std::unique_ptr<ParamVisualizerBase> vis, std::string printName,
	VisualizerManager &vm,
	std::vector<vtkSmartPointer<vtkTextActor> > &textActs, vtkRenderer *ren,
	int textSize, double textVOff, double textVSpacing) {


	visID vid = vm.AddVisualizer(std::move(vis));
	if (vid >= 0) {

		auto t = vtkSmartPointer<vtkTextActor>::New();
		auto s = std::ostringstream{};

		s << int(vid + 1) << ": " << printName;
		t->SetInput(s.str().c_str());
		textActs.push_back(t);

		t->GetTextProperty()->SetColor(0.2, 0.2, 0.2);


		t->GetTextProperty()->SetFontSize(textSize);

		//t->SetTextScaleModeToViewport();
		t->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
		t->GetPositionCoordinate()->SetViewport(ren);
		t->GetPositionCoordinate()->SetValue(0.02, textVOff + textVSpacing * double(vid));

		//t->SetDisplayPosition(12, textVOff + textVSpacing * vid);
		

		ren->AddActor2D(t);
	}
	return vid;
}


vtkSmartPointer<vtkRenderer> MakeRenderer(double x1 = 0.0, double y1 = 0.0, double x2 = 1.0, double y2 = 1.0)
{
	auto ren = vtkSmartPointer<vtkRenderer>::New();
	ren->SetBackground(0.4, 0.4, 0.8);

	ren->SetViewport(x1, y1, x2, y2);

	ren->GetActiveCamera()->ParallelProjectionOn();



	auto overheadLight = vtkSmartPointer<vtkLight>::New();

	overheadLight->SetPosition(0, 0, 4);
	overheadLight->SetExponent(1.2);
	overheadLight->SetIntensity(1);

	ren->AddLight(overheadLight);

	return ren;
}

void fmiVis::AddMapPlane(vtkSmartPointer<vtkRenderer> ren,const std::string &file, nbsMetadata &meta, bool flat) {

	cout << "Loading map..." << endl;

	
	
	auto mapReader = vtkSmartPointer<vtkPNGReader>::New();
	mapReader->SetFileName("bottom_indexed.png");

	mapReader->Update();


	auto texture = vtkSmartPointer<vtkTexture>::New();
	texture->SetInputData(mapReader->GetOutput());
	texture->SetInterpolate(true);
	texture->Update();

	auto mapNbs = new nbsSurface(file, &meta, 1, 13000, flat);

	mapNbs->Update();

	auto mapMap = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapMap->SetInputData(mapNbs->GetOutput());
	mapMap->SetScalarVisibility(false);
	mapMap->Update();

	auto texturedPlane = vtkSmartPointer<vtkActor>::New();
	texturedPlane->SetMapper(mapMap);
	texturedPlane->SetTexture(texture);
	texturedPlane->SetPickable(false);
	texturedPlane->SetDragable(false);

	

	ren->AddActor(texturedPlane);
}

void fmiVis::MakeTimeAnimator(vtkRenderer *ren, fmiVis::ViewportManager &vm,nbsMetadata &meta,
	vtkRenderWindowInteractor *iren, vtkRenderWindow *renWin, VisualizationInteractorImpl &style) {

	auto sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();

	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint1Coordinate()->SetValue(0.1, 0.08);
	sliderRep->GetPoint2Coordinate()->SetValue(0.9, 0.08);

	sliderRep->SetShowSliderLabel(false);

	sliderRep->SetMinimumValue(0);
	sliderRep->SetMaximumValue(meta.timeSteps);
	sliderRep->SetValue(0);


	sliderRep->GetTitleProperty()->SetColor(0.2, 0.2, 0.9);
	sliderRep->GetTitleProperty()->SetFontSize(16);
	sliderRep->GetTitleProperty()->BoldOff();
	sliderRep->GetTitleProperty()->ShadowOff();

	sliderRep->SetTitleText(nbsMetadata::getTimeString(meta.minT).c_str());

	auto slider = vtkSmartPointer<vtkSliderWidget>::New();
	slider->SetInteractor(iren);
	slider->SetRepresentation(sliderRep);
	slider->SetAnimationModeToJump();
	slider->SetCurrentRenderer(ren);

	slider->EnabledOn();

	auto ta = std::make_unique<fmiVis::TimeAnimator>(ren, renWin,slider,&vm,&meta );

	style.setTA(std::move(ta));
}


void fmiVis::MakeFileText(std::string &file, vtkSmartPointer<vtkRenderer> ren)
{

	int *winSize = ren->GetSize();

	auto t = vtkSmartPointer<vtkTextActor>::New();
	auto s = std::ostringstream{};

	int i = 0;
	for (i = file.length() - 1; i >= 0; --i)
		if (file[i] == '/' || file[i] == '\\') break;

	s << file.substr(i + 1);

	t->SetInput(s.str().c_str());

	t->GetTextProperty()->SetColor(0.1, 0.1, 0.1);

	t->GetTextProperty()->SetFontSize(20);

	t->SetTextScaleModeToViewport();
	t->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	t->GetPositionCoordinate()->SetValue(0.02,0.96);

	ren->AddActor2D(t);
}


void MakeVisualizers(std::string file, nbsMetadata & meta, VisualizerManager *visualizerMan,
	std::vector<vtkSmartPointer<vtkTextActor>> *textActs, vtkRenderer *ren,
	int textSize, double textVOff, double textVSpacing,
	std::map<int, std::string> &paramsSurf, NFmiFastQueryInfo &dataInfo)
{


	auto drawParamFactory = fmiVis::LoadOptions();

	visualizerMan->setDrawParamFac(drawParamFactory);

	dataInfo.Param(kFmiWindDirection);
	addVis(std::make_unique<ParamVisualizerWindVec2D>(file, meta, dataInfo.Param(), drawParamFactory.get()), "Wind Vectors"s,
		*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);

	auto iconMapping = [](double in) {
		return static_cast<int> ((100.0 - in) / 100.01 * 3.0);
	};


	dataInfo.Param(kFmiTotalCloudCover);
	addVis(std::make_unique<ParamVisualizerIcon>(file, meta, dataInfo.Param(), drawParamFactory.get(), iconMapping), "Total Cloud Cover (Icon)",
		*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);

	addVis(std::make_unique<ParamVisualizerArrayText>(file, meta, dataInfo.Param(), drawParamFactory.get(), fmiVis::redToGreenColFunc(0, 100)), "Total Cloud Cover (ArrayText)",
		*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);
	addVis(std::make_unique<ParamVisualizerPolyText>(file, meta, dataInfo.Param(), drawParamFactory.get()), "Total Cloud Cover (Polytext)",
		*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);
	addVis(std::make_unique<ParamVisualizerText>(file, meta, dataInfo.Param(), drawParamFactory.get()), "Total Cloud Cover (Text)",
		*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);


	for (auto &parampair : paramsSurf) {
		if (dataInfo.Param(FmiParameterName(parampair.first))) {
			auto s = std::ostringstream{};
			s << parampair.second << "(Surface)"s;

			//auto drawParam = drawParamFactory.get()->CreateDrawParam(dataInfo.Param(),nullptr);
			//dataInfo.Param();
			visID vid = addVis(std::make_unique<ParamVisualizerSurf>(file, meta, visualizerMan->GetLabeler(), dataInfo.Param(), drawParamFactory.get(), true), s.str(),
				*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);
			if (vid == -1)
			{
				cout << "Failed to construct visualizer for " << parampair.second << endl;
				continue;
			}
			visualizerMan->SetCrop(vid, false);
		}
	}


	//visualizerMan->EnableVis(0);
}


void fmiVis::ViewportFactory::MakeTimeGridView(size_t numX, size_t numY,
	std::string file, nbsMetadata &meta, fmiVis::ViewportManagerTimegrid &viewportMan, vtkRenderWindowInteractor *iren, vtkRenderWindow *renWin, VisualizationInteractor2D *style)
{
	auto baseRen = MakeRenderer();
	renWin->AddRenderer(baseRen);
	baseRen->SetLayer(1);

	MakeFileText(file, baseRen);

	viewportMan.SetBaseViewport(baseRen);

	vtkCamera* cam;

	renWin->SetNumberOfLayers(2);

	double viewWidth = 1.0 / numX;
	double viewHeight = 1.0 / numY;

	for (size_t y = 0; y < numY; y++)
		for(size_t x = 0; x < numX; x++)
		{

			double x1 = viewWidth*x;
			double y1 = viewHeight*y;

			double x2 = viewWidth*(x + 1);
			double y2 = viewHeight*(y + 1);

			auto ren = MakeRenderer(x1,y1,x2,y2);

			ren->SetLayer(0);

			if (x == 0 && y == 0) {

				cam = ren->GetActiveCamera();
				cam->SetParallelScale(100);
				cam->SetPosition(100, 100, 100);
				cam->SetFocalPoint(100, 100, 0);

				auto callback = vtkSmartPointer<fmiVis::CameraCallback>::New();
				callback->setManager(&viewportMan );
				cam->AddObserver(vtkCommand::ModifiedEvent, callback);
			}
			else {
				ren->SetActiveCamera(cam);
			}


			renWin->AddRenderer(ren);

			std::unique_ptr<VisualizerManager> visualizerMan = std::make_unique<VisualizerManager>(ren, meta);



			AddMapPlane(ren, file, meta);

			cout << "Initializing visualizers..." << endl;


			auto textActs = std::make_unique< std::vector<vtkSmartPointer<vtkTextActor> > >();

			auto data = NFmiQueryData(file);
			auto dataInfo = NFmiFastQueryInfo(&data);

			int *winSize = ren->GetSize();


			auto textSize = 14;
			auto textVOff = 1.0 - 0.1;
			auto textVSpacing = -0.04;


			MakeVisualizers(file, meta, visualizerMan.get(), textActs.get(), ren, textSize, textVOff, textVSpacing, paramsSurf, dataInfo);

			viewportMan.AddViewport(ren, std::move(visualizerMan), std::move(textActs));
		}

	MakeTimeAnimator(baseRen,viewportMan, meta,
		iren, renWin, style->GetImpl());
}

