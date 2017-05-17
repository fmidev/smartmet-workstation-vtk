#include <math.h>

#include <list>



#include <vtkVersion.h>
#include <vtkSmartPointer.h>

#include <vtkOutputWindow.h>

#include <vtkWin32OutputWindow.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>

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


#include "fmiVisInteractor.h"


#include "VisualizerManager.h"
#include "VisualizerFactory.h"

#include "CreateHeightdata.h"

#include "TimeAnimator.h"

#include "Windbarb.h"
#include "ParamVisualizerWindVec.h"



static std::string *newBaseFile;


//Callback jota kutsutaan kun laatikkoa skaalataan
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
	VisualizerManager *vm;
};



int main(int argc, char *argv[])
{
	cout << "Initializing VTK..." << endl;

	//lisää aikasarjatuen?
	auto *sdp = vtkStreamingDemandDrivenPipeline::New();
	vtkAlgorithm::SetDefaultExecutivePrototype(sdp);
	sdp->Delete();

	//vtk boilerplatea
	auto renWin = vtkSmartPointer<vtkRenderWindow>::New();
	auto ren1 = vtkSmartPointer<vtkRenderer>::New();
	
	//taustaväri
	ren1->SetBackground(0.4, 0.4, 0.8);

	renWin->AddRenderer(ren1);

	//renWin->SetFullScreen(true);
	renWin->SetSize(800, 800);

	vtkWin32OutputWindow::SafeDownCast(vtkOutputWindow::GetInstance())->SendToStdErrOn();


	

	auto iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);


	ren1->GetActiveCamera()->Azimuth(45);
	ren1->GetActiveCamera()->SetViewUp(0,0,1);

	renWin->Render();


	cout << "Initializing interface..." << endl;

	VisualizerFactory::init();

	std::string file = std::string( argc<2 ? 
		"D:/3D-dataa/201703210327_hirlam_skandinavia_mallipinta.sqd" :
		argv[1]
	);



	VisualizerManager vm {ren1};

	vm.GetMeta().init(file);


	auto meta = vm.GetMeta();



	auto style = vtkSmartPointer<fmiVisInteractor>::New();

	style->setVM(&vm);

	iren->SetInteractorStyle(style);

	//koordinaattiakselit
	auto cubeAxesActor = vtkSmartPointer<vtkCubeAxesActor>::New();
	double bounds[6] = { 0,meta.sizeX*2,0,meta.sizeY*2,0,80*2};
	cubeAxesActor->SetBounds(bounds);
	cubeAxesActor->SetCamera(ren1->GetActiveCamera());

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

	ren1->AddActor(cubeAxesActor);
	

	//luetaan kartta levyltä ja mapataan se tasolle

	cout << "Loading map..." << endl;

	auto mapReader = vtkSmartPointer<vtkPNGReader>::New();
	mapReader->SetFileName("bottom_indexed.png");

	mapReader->Update();

	auto texture = vtkSmartPointer<vtkTexture>::New();
	texture->SetInputConnection(mapReader->GetOutputPort());

	double* volumeBounds = new double[6];
	volumeBounds = cubeAxesActor->GetBounds();
	double* planeScale = new double[2];

	planeScale[0] = volumeBounds[1] - volumeBounds[0];
	planeScale[1] = volumeBounds[2] - volumeBounds[3];

	auto planeData = CreateHeightdata(file);

	auto texturePlane = vtkSmartPointer<vtkTextureMapToPlane>::New();
	texturePlane->SetInputData( planeData );

	auto planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

	planeMapper->SetInputData(planeData);

	//double* planePos = new double[2];
	//planePos[0] = planeBounds[1] / 2.0;
	//planePos[1] = planeBounds[3] / 2.0;


	auto texturedPlane = vtkSmartPointer<vtkActor>::New();
	texturedPlane->SetMapper(planeMapper);
	texturedPlane->SetTexture(texture);

	texturedPlane->SetMapper(planeMapper);
	texturedPlane->SetDragable(false);
	texturedPlane->SetPickable(false);

	ren1->AddActor(texturedPlane);

	cout << "Initializing widgets..." << endl;

	auto boxRep = vtkSmartPointer<vtkBoxRepresentation>::New();
	boxRep->SetPlaceFactor(1.1);
	boxRep->PlaceWidget(cubeAxesActor->GetBounds() );
	boxRep->InsideOutOn();
	boxRep->GetSelectedFaceProperty()->SetOpacity(0.0);

	auto boxWidget = vtkSmartPointer<vtkBoxWidget2>::New();
	boxWidget->SetInteractor(iren);
	boxWidget->SetRepresentation(boxRep);

	boxWidget->SetTranslationEnabled(false);
	boxWidget->SetRotationEnabled(false);

	boxWidgetCallback *callback = boxWidgetCallback::New();

	callback->setManager( &vm );

	boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);
	callback->Delete();




	auto planeWidget = vtkSmartPointer<vtkPlaneWidget>::New();
	planeWidget->SetInteractor(iren);
	planeWidget->SetResolution(80);
	planeWidget->SetRepresentationToOutline();
	planeWidget->SetNormalToZAxis(true);
	planeWidget->SetProp3D(texturedPlane);
	planeWidget->GetHandleProperty()->SetPointSize(0.2);
	planeWidget->GetSelectedHandleProperty()->SetPointSize(0.1);

	
	cout << "Initializing visualizers..." << endl;

	std::map<int,std::string> params3D = {
		std::pair<int,std::string>(kFmiWindSpeedMS,"kFmiWindSpeedMS"),
		std::pair<int,std::string>(kFmiTotalCloudCover,"kFmiTotalCloudCover"),
		std::pair<int,std::string>(kFmiCloudWater,"kFmiCloudWater" ),
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
	std::map<int, visID> paramVID;

	std::vector<vtkSmartPointer<vtkTextActor> > textActs;

	auto data = NFmiQueryData(file);
	auto dataInfo = NFmiFastQueryInfo(&data);

	int *winSize = ren1->GetSize();


	visID vid = vm.AddVisualizer(std::make_unique<ParamVisualizerWindVec>(file, meta, planeWidget->GetPolyDataAlgorithm()->GetOutputPort()));
	paramVID.insert(std::pair<int, visID>(ParamVisualizerWindVec::PARAM_WINDVEC, vid));


	auto t = vtkSmartPointer<vtkTextActor>::New();
	std::ostringstream s;
	s << int(vid + 1) << ": " << "Wind vectors";
	t->SetInput(s.str().c_str());
	textActs.push_back(t);

	t->GetTextProperty()->SetColor(0.8, 0.8, 0.8);
	

	t->GetTextProperty()->SetFontSize(7);

	t->SetTextScaleModeToViewport();
	t->SetDisplayPosition(10, winSize[1] - 20 - 10 * vid);

	ren1->AddActor2D(t);


	for (auto &parampair : params3D) {
		if (dataInfo.Param(FmiParameterName(parampair.first) ) ) {
			visID vid = vm.AddVisualizer(VisualizerFactory::make3DVisualizer(file, meta, parampair.first));
			if (vid == -1) continue;
			paramVID.insert(std::pair<int, visID>(parampair.first, vid));


			auto t = vtkSmartPointer<vtkTextActor>::New();
			std::ostringstream s;
			s << int(vid+1) << ": " << parampair.second;
			t->SetInput(s.str().c_str());
			textActs.push_back(t);

			vm.DisableVis(vid);
			t->GetTextProperty()->SetColor(0.2, 0.2, 0.2);


			t->GetTextProperty()->SetFontSize(7);

			t->SetTextScaleModeToViewport();
			t->SetDisplayPosition(10, winSize[1]-20 - 10 * vid);

			ren1->AddActor2D(t);
		}
	}

	

	for (auto &parampair : params2D) {
		if (dataInfo.Param(FmiParameterName(parampair.first))) {
			visID vid = vm.AddVisualizer(VisualizerFactory::make2DVisualizer(file, meta, planeWidget->GetPolyDataAlgorithm()->GetOutputPort(),vm.GetLabeler(), parampair.first));
			if (vid == -1) continue;
			paramVID.insert(std::pair<int, visID>(parampair.first, vid));
			vm.SetCrop(vid, false);
			vm.DisableVis(vid);

			auto t = vtkSmartPointer<vtkTextActor>::New();
			std::ostringstream s;
			s << int(vid+1) << ": " << parampair.second;
			t->SetInput(s.str().c_str());
			textActs.push_back(t);
			t->GetTextProperty()->SetColor(0.2, 0.2, 0.2);
			t->GetTextProperty()->SetFontSize(7);


			t->SetTextScaleModeToViewport();
			t->SetDisplayPosition(10, winSize[1]-20 - 10 * vid);
			

			ren1->AddActor2D(t);
		}
	}

	style->setVisTexts(&textActs);


	planeWidgetCallback *planeCallback = planeWidgetCallback::New();

	planeCallback->setManager(&vm);


	planeWidget->PlaceWidget();


	planeWidget->AddObserver(vtkCommand::InteractionEvent, planeCallback);

	planeWidget->EnabledOn();


	vtkSliderRepresentation2D * sliderRep = vtkSliderRepresentation2D::New();

	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint1Coordinate()->SetValue(0.1, 0.08);
	sliderRep->GetPoint2Coordinate()->SetValue(0.9, 0.08);

	sliderRep->SetShowSliderLabel(false);

	sliderRep->SetMinimumValue( 0 );
	sliderRep->SetMaximumValue(meta.timeSteps);
	sliderRep->SetValue( 0 );


	sliderRep->GetTitleProperty()->SetColor(0.2, 0.2, 0.9);
	sliderRep->GetTitleProperty()->SetFontSize(10);
	sliderRep->GetTitleProperty()->BoldOff();
	sliderRep->GetTitleProperty()->ShadowOff();

	sliderRep->SetTitleText( nbsMetadata::getTimeString(meta.minT).c_str() );

	vtkSliderWidget *slider = vtkSliderWidget::New();
	slider->SetInteractor(iren);
	slider->SetRepresentation(sliderRep);
	slider->SetAnimationModeToJump();

	slider->EnabledOn();





	auto ta = TimeAnimator{ renWin,slider,&vm,&meta };

	style->setTA(&ta);

	auto overheadLight = vtkSmartPointer<vtkLight>::New();

	overheadLight->SetPosition(0, 0, volumeBounds[5]*1.1);
	overheadLight->SetExponent(2);
	overheadLight->SetIntensity(0.3);

	ren1->AddLight(overheadLight);


	ren1->Render();
	ren1->ResetCamera();

	iren->Initialize();
	iren->Start();


	return EXIT_SUCCESS;
}