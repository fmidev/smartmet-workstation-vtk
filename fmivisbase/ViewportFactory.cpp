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
#include "VisualizerFactory.h"
#include "ParamVisualizerIcon.h"
#include "ParamVisualizerArrayText.h"
#include "ParamVisualizerPolyText.h"
#include "ParamVisualizerText.h"

#include "ViewportManager.h"
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

#include "fmiVisInteractor.h"

using namespace std::string_literals;
using namespace fmiVis;

visID addVis(std::unique_ptr<ParamVisualizerBase> vis, std::string printName,
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
		t->SetDisplayPosition(10, textVOff + textVSpacing * vid);

		ren->AddActor2D(t);
	}
	return vid;
}


vtkSmartPointer<vtkRenderer> MakeDefaultRenderer()
{
	auto ren = vtkSmartPointer<vtkRenderer>::New();
	ren->SetBackground(0.4, 0.4, 0.8);

	ren->GetActiveCamera()->ParallelProjectionOn();



	auto overheadLight = vtkSmartPointer<vtkLight>::New();

	overheadLight->SetPosition(0, 0, 4);
	overheadLight->SetExponent(1.2);
	overheadLight->SetIntensity(0.6);

	ren->AddLight(overheadLight);

	return ren;
}

void addMapPlane(vtkSmartPointer<vtkRenderer> ren,const std::string &file, nbsMetadata &meta) {

	cout << "Loading map..." << endl;

	auto mapReader = vtkSmartPointer<vtkPNGReader>::New();
	mapReader->SetFileName("bottom_indexed.png");

	mapReader->Update();


	auto texture = vtkSmartPointer<vtkTexture>::New();
	texture->SetInputData(mapReader->GetOutput());
	texture->SetInterpolate(true);
	texture->Update();

	auto mapNbs = new nbsSurface(file, &meta, 1, 13000, true);

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

void MakeTimeAnimator(VisualizerManager &vm,nbsMetadata &meta,
	vtkRenderWindowInteractor *iren, vtkRenderWindow *renWin, fmiVisInteractor2D *style) {

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

	slider->EnabledOn();

	auto ta = std::make_unique<::TimeAnimator>( renWin,slider,&vm,&meta );

	style->setTA(std::move(ta));
}

void fmiVis::ViewportFactory::MakeSingleView(std::string file, nbsMetadata &meta, ViewportManager &viewportMan,
	vtkRenderWindowInteractor *iren, vtkRenderWindow *renWin, fmiVisInteractor2D *style) {

	auto ren = MakeDefaultRenderer();

	renWin->AddRenderer(ren);

	std::unique_ptr<VisualizerManager> visualizerMan = std::make_unique<VisualizerManager>(ren, meta);



	addMapPlane(ren, file, meta);

	cout << "Initializing visualizers..." << endl;

	std::map<int, std::string> paramsSurf = {
		std::pair<int,std::string>(kFmiPressure,"kFmiPressure"),
		std::pair<int,std::string>(kFmiTemperature,"kFmiTemperature"),
		std::pair<int,std::string>(kFmiHumidity,"kFmiHumidity"),
		std::pair<int,std::string>(kFmiWindSpeedMS,"kFmiWindSpeedMS"),
		std::pair<int,std::string>(kFmiTotalCloudCover,"kFmiTotalCloudCover"),
		std::pair<int,std::string>(kFmiPrecipitation1h,"kFmiPrecipitation1h"),
	};

	auto textActs = std::make_unique< std::vector<vtkSmartPointer<vtkTextActor> > >();

	auto data = NFmiQueryData(file);
	auto dataInfo = NFmiFastQueryInfo(&data);

	int *winSize = ren->GetSize();



	auto t = vtkSmartPointer<vtkTextActor>::New();
	auto s = std::ostringstream{};

	auto textSize = 14;
	auto textVOff = winSize[1] - 50;
	auto textVSpacing = -16;


	int i = 0;
	for (i = file.length() - 1; i >= 0; --i)
	if (file[i] == '/' || file[i] == '\\') break;

	s << file.substr(i + 1);

	t->SetInput(s.str().c_str());

	t->GetTextProperty()->SetColor(0.1, 0.1, 0.1);

	t->GetTextProperty()->SetFontSize(20);

	//t->SetTextScaleModeToViewport();
	t->SetDisplayPosition(10, winSize[1] - 25);


	ren->AddActor2D(t);



	addVis(std::make_unique<ParamVisualizerWindVec2D>(file, meta), "Wind Vectors"s,
		*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);

	auto iconMapping = [](double in) {
		return static_cast<int> ((100.0 - in) / 100.01 * 3.0);
	};

	addVis(std::make_unique<ParamVisualizerIcon>(file, meta, kFmiTotalCloudCover, iconMapping), "Total Cloud Cover (Icon)",
		*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);

	addVis(std::make_unique<ParamVisualizerArrayText>(file, meta, kFmiTotalCloudCover, VisualizerFactory::greenToRedColor(0, 100)), "Total Cloud Cover (ArrayText)",
		*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);
	addVis(std::make_unique<ParamVisualizerPolyText>(file, meta, kFmiTotalCloudCover), "Total Cloud Cover (Polytext)",
		*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);
	addVis(std::make_unique<ParamVisualizerText>(file, meta, kFmiTotalCloudCover), "Total Cloud Cover (Text)",
		*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);

	for (auto &parampair : paramsSurf) {
		if (dataInfo.Param(FmiParameterName(parampair.first))) {
			s = std::ostringstream{};
			s << parampair.second << "(Surface)"s;

			visID vid = addVis(VisualizerFactory::makeSurfVisualizer(file, meta, visualizerMan->GetLabeler(), parampair.first, true), s.str(),
				*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);
			if (vid == -1)
			{
				cout << "Failed to construct visualizer for " << parampair.second << endl;
				continue;
			}
			visualizerMan->SetCrop(vid, false);
		}
	}


	visualizerMan->EnableVis(0);


	style->setVisTexts(textActs.get());

	MakeTimeAnimator(*visualizerMan, meta,
		iren, renWin, style);

	viewportMan.AddViewport(ren,std::move(visualizerMan),std::move(textActs));


}

