
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkWin32OutputWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <VisualizerManager.h>
#include <vtkImageMapper3D.h>
#include <vtkPNGReader.h>
#include <vtkTexture.h>
#include <nbsSurface.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkActor.h>
#include <vtkImageActor.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkBoxRepresentation.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <TimeAnimator.h>
#include <vtkLight.h>

#include "Interactor.h"

#include "ViewportFactory.h"

#include "ViewportManagerTimegrid.h"

using namespace std::string_literals;


int main(size_t argc, char* argv[])
{
	cout << "Initializing VTK..." << endl;

	//lisää aikasarjatuen?
	auto *sdp = vtkStreamingDemandDrivenPipeline::New();
	vtkAlgorithm::SetDefaultExecutivePrototype(sdp);
	sdp->Delete();

	//vtk boilerplatea
	auto renWin = vtkSmartPointer<vtkRenderWindow>::New();
	//auto ren1 = vtkSmartPointer<vtkRenderer>::New();

	//taustaväri
	//ren1->SetBackground(0.4, 0.4, 0.8);

	//renWin->AddRenderer(ren1);

	//renWin->SetFullScreen(true);
	renWin->SetSize(800, 800);

	vtkWin32OutputWindow::SafeDownCast(vtkOutputWindow::GetInstance())->SendToStdErrOn();

	auto iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	//ren1->GetActiveCamera()->ParallelProjectionOn();

	//renWin->Render();

	cout << "Initializing interface..." << endl;


	std::string file = std::string(argc < 2 ?
		"D:/3D-dataa/201706160543_gfs_scandinavia_surface.sqd" :
		argv[1]
	);

	//VisualizerManager vm{ ren1 };

	//vm.GetMeta().init(file);


	auto meta = nbsMetadata();
	meta.init(file);



	auto style = vtkSmartPointer<fmiVis::VisualizationInteractor2D>::New();

	//style->setVM(&vm);

	iren->SetInteractorStyle(style);

	size_t sizeX = 6;
	size_t sizeY = 3;

	auto vm = fmiVis::ViewportManagerTimegrid{ sizeX,sizeY };

	fmiVis::ViewportFactory::MakeTimeGridView(sizeX, sizeY,file, meta, vm, iren, renWin, style);

	style->setVM(&vm);

	iren->Initialize();
	iren->Start();



	return EXIT_SUCCESS;
}


