#include "VolVis.h"

#include <string>

#include <vtkOutputWindow.h>

#include <vtkWin32OutputWindow.h>


#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkBMPReader.h>
#include <vtkPNGReader.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>

#include <vtkPolyDataMapper.h>

#include <vtkCubeAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

#include <vtkPlanes.h>

#include <vtkBoxWidget2.h>
#include <vtkBoxRepresentation.h>

#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>

#include <vtkPlaneWidget.h>
#include <vtkLinearSubdivisionFilter.h>

#include <vtkLight.h>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>
#include <NFmiParameterName.h>

#include <vtkTransform.h>
#include <vtkPolyDataNormals.h>
#include <vtkTransformPolyDataFilter.h>


#include "DrawOptions.h"
#include <vtkCubeAxesActor.h>
#include <vtkPolyDataNormals.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTexture.h>
#include <vtkPNGReader.h>
#include "nbsSurface.h"
#include <vtkPolyDataMapper.h>

#include <vtkLight.h>

#include "ViewportFactory.h"
#include "ParamVisualizerWindVec2D.h"
#include "ParamVisualizerSurf.h"
#include "ParamVisualizer3D.h"
#include "ParamVisualizer2D.h"


#include "DrawOptions.h"
#include "ParamVisualizerWindVec.h"

using namespace std::string_literals;

namespace fmiVis {
	std::map<int, Vis3DParams> config3D;
	std::map<int, Vis2DParams> config2D;
}

using namespace fmiVis;

class boxWidgetCallback : public vtkCommand
{
public:
	static boxWidgetCallback *New()
	{
		return new boxWidgetCallback;
	}
	void Execute(vtkObject *caller, unsigned long, void*) VTK_OVERRIDE
	{
		vtkBoxWidget2 *widget = reinterpret_cast<vtkBoxWidget2*>(caller);
		vtkBoxRepresentation* boxRep = reinterpret_cast<vtkBoxRepresentation*>(widget->GetRepresentation());

		boxRep->GetPlanes(planes);
		vm->CropMappers(planes);
	}
	void setManager(VisualizerManager *vm)
	{
		this->vm = vm;
	}

protected:
	boxWidgetCallback() {
		planes = vtkSmartPointer<vtkPlanes>::New();
	}
	VisualizerManager *vm;
	vtkSmartPointer<vtkPlanes> planes;
};

class planeWidgetCallback : public vtkCommand
{
public:
	planeWidgetCallback() {}

	static planeWidgetCallback *New()
	{
		return new planeWidgetCallback;
	}
	void Execute(vtkObject *caller, unsigned long, void*) VTK_OVERRIDE
	{
		vm->Update();
	}
	void setManager(VisualizerManager *vm)
	{
		this->vm = vm;
	}


protected:
	VisualizerManager * vm;
};

void MakeVisualizers3D(std::string file, std::string drawParamPath, std::string surfFile, nbsMetadata & meta, VisualizerManager *visualizerMan,
	std::vector<vtkSmartPointer<vtkTextActor>> *textActs, vtkRenderer *ren, vtkAlgorithmOutput* probe,
	int textSize, double textVOff, double textVSpacing, std::map<int, std::string> &paramsSurf, NFmiFastQueryInfo &dataInfo)
{

	std::map<int, std::string> params3D = {
		std::pair<int,std::string>(kFmiWindSpeedMS,"kFmiWindSpeedMS"),
		std::pair<int,std::string>(kFmiTotalCloudCover,"kFmiTotalCloudCover"),
		std::pair<int,std::string>(kFmiCloudWater,"kFmiCloudWater"),
		std::pair<int,std::string>(277,"cloud-ice"),
		std::pair<int,std::string>(kFmiIcing,"kFmiIcing"),
		std::pair<int,std::string>(kFmiClearAirTurbulence,"kFmiClearAirTurbulence"),
		std::pair<int,std::string>(kFmiClearAirTurbulence2,"kFmiClearAirTurbulence2"),
	};
	std::map<int, std::string> params2D = {
		std::pair<int,std::string>(kFmiTemperature,"kFmiTemperature"),
		std::pair<int,std::string>(kFmiPotentialTemperature,"kFmiPotentialTemperature"),
		std::pair<int,std::string>(kFmiHumidity,"kFmiHumidity"),
		std::pair<int,std::string>(kFmiWindSpeedMS,"kFmiWindSpeedMS"),
		std::pair<int,std::string>(kFmiVerticalVelocityMMS,"kFmiVerticalVelocityMMS"),
	};


	auto drawParamFactory = fmiVis::LoadOptions(drawParamPath);

	visualizerMan->setDrawParamFac(drawParamFactory);

	dataInfo.Param(kFmiWindDirection);

	auto vid = fmiVis::addVis(std::make_unique<ParamVisualizerWindVec>(file, meta, dataInfo.Param(), drawParamFactory.get(), probe),
				"Wind Vectors"s,*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);
	visualizerMan->SetCrop(vid, false);
	for (auto &parampair : params3D) {
		if (dataInfo.Param(FmiParameterName(parampair.first))) {

			auto s = std::ostringstream{};
			s << parampair.second << " ( Volume )";
			visID vid = -1;

			auto iter = fmiVis::config3D.find(parampair.first);

			if (iter != fmiVis::config3D.end()) {
				auto config = iter->second;
				vid = fmiVis::addVis(
					std::make_unique<ParamVisualizer3D>(
						file, meta, dataInfo.Param(), drawParamFactory.get(),
						vtkColorTransferFunction::SafeDownCast(config.color), config.opacity, config.contourThreshold,
						config.contourColor, config.contourOpacity),
					s.str(), *visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);
			}
			if (vid == -1)
			{
				cout << "Failed to construct visualizer for " << parampair.second << endl;
				continue;
			}
			visualizerMan->SetCrop(vid, true);
		}
	}

	for (auto &parampair : params2D) {
		if (dataInfo.Param(FmiParameterName(parampair.first))) {

			auto s = std::ostringstream{};
			s << parampair.second << " ( Plane )";

			visID vid = -1;

			auto iter = fmiVis::config2D.find(parampair.first);

			if (iter != fmiVis::config2D.end()) {
				auto config = iter->second;
				vid = fmiVis::addVis(
						std::make_unique<ParamVisualizer2D>(
							file, meta, dataInfo.Param(), drawParamFactory.get(), probe,
							config.contourColors, visualizerMan->GetLabeler(),config.range,config.numContours),
					s.str(), *visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);
			}
			if (vid == -1)
			{
				cout << "Failed to construct visualizer for " << parampair.second << endl;
				continue;
			}
			visualizerMan->SetCrop(vid, false);
		}
	}

	if (surfFile.length() > 0) {

		for (auto &parampair : paramsSurf) {
			if (dataInfo.Param(FmiParameterName(parampair.first))) {
				auto s = std::ostringstream{};
				s << parampair.second << "(Surface)"s;

				//auto drawParam = drawParamFactory.get()->CreateDrawParam(dataInfo.Param(),nullptr);
				//dataInfo.Param();
				visID vid = fmiVis::addVis(std::make_unique<ParamVisualizerSurf>(surfFile, meta, visualizerMan->GetLabeler(), dataInfo.Param(), drawParamFactory.get(), false), s.str(),
					*visualizerMan, *textActs, ren, textSize, textVOff, textVSpacing);
				if (vid == -1)
				{
					cout << "Failed to construct visualizer for " << parampair.second << endl;
					continue;
				}
				visualizerMan->SetCrop(vid, false);
			}
		}
	}


	//visualizerMan->EnableVis(0);
}


fmiVis::Widgets3D fmiVis::Make3DView(std::string file, std::string surfFile, std::string drawParamPath, nbsMetadata &meta, ViewportManager &viewportMan,
	vtkRenderWindowInteractor *iren, vtkRenderWindow *renWin, VisualizationInteractor *style) {

	auto ren = renWin->GetRenderers()->GetFirstRenderer();

	//taustaväri
	ren->SetBackground(0.4, 0.4, 0.8);

	//renWin->SetFullScreen(true);
	renWin->SetSize(800, 800);

	vtkWin32OutputWindow::SafeDownCast(vtkOutputWindow::GetInstance())->SendToStdErrOn();


	iren->SetRenderWindow(renWin);


	ren->GetActiveCamera()->Azimuth(45);
	ren->GetActiveCamera()->SetViewUp(0, 0, 1);

	renWin->Render();


	//auto cam = ren->GetActiveCamera();
	//auto callback = vtkSmartPointer<fmiVis::CameraCallback>::New();
	//callback->setManager(&viewportMan);
	//cam->AddObserver(vtkCommand::ModifiedEvent, callback);

	std::unique_ptr<VisualizerManager> visualizerMan = std::make_unique<VisualizerManager>(ren, drawParamPath, meta);

	auto cubeAxesActor = vtkSmartPointer<vtkCubeAxesActor>::New();
	double bounds[6] = { 0,meta.sizeX * 2,0,meta.sizeY * 2,0,80 * 2 };
	cubeAxesActor->SetBounds(bounds);
	cubeAxesActor->SetCamera(ren->GetActiveCamera());

	cubeAxesActor->GetTitleTextProperty(0)->SetColor(1.0, 0.0, 0.0);
	cubeAxesActor->GetLabelTextProperty(0)->SetColor(1.0, 0.0, 0.0);

	cubeAxesActor->SetXAxisRange(meta.p1.Y(), meta.p2.Y());
	cubeAxesActor->SetXTitle("longitude");

	cubeAxesActor->GetTitleTextProperty(1)->SetColor(0.0, 1.0, 0.0);
	cubeAxesActor->GetLabelTextProperty(1)->SetColor(0.0, 1.0, 0.0);

	cubeAxesActor->SetYAxisRange(meta.p1.X(), meta.p2.X());
	cubeAxesActor->SetYTitle("latitude");

	cubeAxesActor->GetTitleTextProperty(2)->SetColor(0.0, 0.0, 1.0);
	cubeAxesActor->GetLabelTextProperty(2)->SetColor(0.0, 0.0, 1.0);

	cubeAxesActor->SetZAxisRange(meta.minH, meta.maxH);
	cubeAxesActor->SetZTitle("m");

	cubeAxesActor->SetDragable(false);
	cubeAxesActor->SetPickable(false);

	ren->AddActor(cubeAxesActor);

	cout << "Initializing visualizers..." << endl;


	//luetaan kartta levyltä ja mapataan se tasolle

	cout << "Loading map..." << endl;

	auto mapReader = vtkSmartPointer<vtkPNGReader>::New();
	mapReader->SetFileName("bottom_indexed.png");
	
	mapReader->Update();



	auto texture = vtkSmartPointer<vtkTexture>::New();

	texture->SetInputData(mapReader->GetOutput());
	texture->SetInterpolate(true);
	texture->Update();

	double* volumeBounds = new double[6];
	volumeBounds = cubeAxesActor->GetBounds();
	double* planeScale = new double[2];

	planeScale[0] = volumeBounds[1] - volumeBounds[0];
	planeScale[1] = volumeBounds[2] - volumeBounds[3];
	nbsSurface* mapNbs;

	if(surfFile.length()>0)
		mapNbs = new nbsSurface(surfFile, &meta, 1);
	else 
		mapNbs = new nbsSurface(file, &meta, 1);
	mapNbs->Update();

	auto mapTransform = vtkSmartPointer<vtkTransform>::New();

	mapTransform->Scale(1, 1, 0.95);
	mapTransform->Translate(0, 0, -0.05);

	auto mapTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

	mapTransformFilter->SetInputData(mapNbs->GetOutput());
	mapTransformFilter->SetTransform(mapTransform);

	mapTransformFilter->Update();

	auto normals = vtkSmartPointer<vtkPolyDataNormals>::New();
	normals->SetInputData(mapTransformFilter->GetOutput());

	normals->Update();

	auto mapMap = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapMap->SetInputData(normals->GetOutput());
	mapMap->SetScalarVisibility(false);
	mapMap->Update();

	auto texturedPlane = vtkSmartPointer<vtkActor>::New();
	texturedPlane->SetMapper(mapMap);
	texturedPlane->SetTexture(texture);
	texturedPlane->SetPickable(false);
	texturedPlane->SetDragable(false);

	ren->AddActor(texturedPlane);

	auto boxRep = vtkSmartPointer<vtkBoxRepresentation>::New();
	boxRep->SetPlaceFactor(1.1);
	boxRep->PlaceWidget(cubeAxesActor->GetBounds());
	boxRep->InsideOutOn();
	boxRep->GetSelectedFaceProperty()->SetOpacity(0.0);

	auto boxWidget = vtkSmartPointer<vtkBoxWidget2>::New();
	boxWidget->SetInteractor(iren);
	boxWidget->SetRepresentation(boxRep);

	boxWidget->SetTranslationEnabled(false);
	boxWidget->SetRotationEnabled(false);
	boxWidget->SetScalingEnabled(false);

	boxWidgetCallback *callback = boxWidgetCallback::New();

	callback->setManager(visualizerMan.get());

	boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);
	callback->Delete();


	auto planeWidget = vtkSmartPointer<vtkPlaneWidget>::New();
	planeWidget->SetInteractor(iren);
	planeWidget->SetResolution(80);
	planeWidget->SetRepresentationToOutline();
	planeWidget->SetNormalToZAxis(true);
	planeWidget->SetPoint1(40, 0, 0);
	planeWidget->SetPoint2(0, 40, 0);
	planeWidget->GetHandleProperty()->SetPointSize(0.2);
	planeWidget->GetSelectedHandleProperty()->SetPointSize(0.1);

	planeWidgetCallback *planeCallback = planeWidgetCallback::New();

	planeCallback->setManager(visualizerMan.get());


	planeWidget->PlaceWidget();


	planeWidget->AddObserver(vtkCommand::InteractionEvent, planeCallback);

	planeWidget->EnabledOn();


	auto textActs = std::make_unique< std::vector<vtkSmartPointer<vtkTextActor> > >();

	auto data = NFmiQueryData(file);
	auto dataInfo = NFmiFastQueryInfo(&data);

	int *winSize = ren->GetSize();

	vtkSmartPointer<vtkTextActor> t;
	std::ostringstream s;

	auto textSize = 14;
	auto textVOff = 0.95;
	auto textVSpacing = -0.02;

	fmiVis::MakeFileText(file, ren);


	MakeVisualizers3D(file, drawParamPath, surfFile, meta, visualizerMan.get(), textActs.get(), ren, planeWidget->GetPolyDataAlgorithm()->GetOutputPort(), textSize, textVOff, textVSpacing, ViewportFactory::paramsSurf, dataInfo);


	style->setVisTexts(textActs.get());


	MakeTimeAnimator(ren, viewportMan, meta,
		iren, renWin, style->GetImpl());

	viewportMan.AddViewport(ren, std::move(visualizerMan), std::move(textActs));

	auto overheadLight = vtkSmartPointer<vtkLight>::New();

	overheadLight->SetPosition(0, 0, volumeBounds[5] * 1.1);
	overheadLight->SetExponent(2);
	overheadLight->SetIntensity(0.3);

	ren->AddLight(overheadLight);

	return Widgets3D{ planeWidget,boxWidget };
}



void fmiVis::InitConfig() {

	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiWindSpeedMS, { kFmiWindSpeedMS,
		fmiVis::redToGreenColFunc(0, 80),
		fmiVis::opacityFunction(0, 80, 20),
		{ 1,0,1 },
		50,
		1
		}));


	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiTotalCloudCover, { kFmiTotalCloudCover,
		fmiVis::redToGreenColFunc(0, 100),
		fmiVis::opacityFunction(0, 100, 20),
		{ 0.6,0.6,0.6 },
		50,
		0.6
		}));

	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiCloudWater, { kFmiCloudWater,
		fmiVis::redToGreenColFunc(0, 0.5),
		fmiVis::opacityFunction(0, 0.5, 0.2),
		{ 1,0,0.5 },
		0.1,
		0.7
		}));

	config3D.insert(std::pair<int, Vis3DParams>(
		277, { 277, //cloud-ice
		fmiVis::redToGreenColFunc(0, 0.3),
		fmiVis::opacityFunction(0, 0.3, 0.01),
		{ 0.4,0,1 },
		0.08,
		0.9
		}));

	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiIcing, { kFmiIcing,
		fmiVis::blueToRedColFunc(0, 9),
		fmiVis::opacityFunction(0, 9, 2),
		{ 0.2,0,1 },
		4,
		1.0
		}));


	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiClearAirTurbulence, { kFmiClearAirTurbulence,
		fmiVis::redToGreenColFunc(0, 150),
		fmiVis::opacityFunction(0, 150, 30),
		{ 1,0,0 },
		40,
		0.5
		}));
	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiClearAirTurbulence2, { kFmiClearAirTurbulence2,
		fmiVis::redToGreenColFunc(0, 150),
		opacityFunction(0, 150, 30),
		{ 1,0,0 },
		40,
		0.5
		}));



	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiPressure, { kFmiPressure ,
		fmiVis::blueToRedColFunc(0,1000),
		{ 0,1000 },
		12
		}));
	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiTemperature, { kFmiTemperature ,
		fmiVis::blueToRedColFunc(-80,30),
		{ -80,30 },
		15
		}));
	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiPotentialTemperature, { kFmiPotentialTemperature ,
		fmiVis::blueToRedColFunc(200,1500),
		{ 200,1500 },
		8
		}));
	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiHumidity, { kFmiHumidity ,
		fmiVis::redToGreenColFunc(0,100),
		{ 0,100 },
		5
		}));
	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiWindSpeedMS, { kFmiWindSpeedMS ,
		fmiVis::blueToRedColFunc(0,50),
		{ 0,50 },
		10
		}));
	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiVerticalVelocityMMS, { kFmiVerticalVelocityMMS ,
		fmiVis::blueToRedColFunc(-200,200),
		{ -200,200 },
		8
		}));

	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiTotalCloudCover, { kFmiTotalCloudCover ,
		fmiVis::blueToRedColFunc(0,100),
		{ 0,100 },
		6
		}));
	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiPrecipitation1h, { kFmiPrecipitation1h ,
		fmiVis::blueToRedColFunc(0,50),
		{ 0,50 },
		8
		}));

}