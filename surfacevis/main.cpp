
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

	std::string surfFile = "D:/3D-dataa/201703210327_hirlam_skandinavia_mallipinta.sqd"s;

	std::string drawParamPath = "D:/3D-dataa/DrawParams";

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0) {
			if (i < argc - 1)
				surfFile = std::string{ argv[i + 1] };
			else cout << "Usage: -s <surface data file>" << endl;
		}
		if (strcmp(argv[i], "-d") == 0) {
			if (i < argc - 1)
				drawParamPath = std::string{ argv[i + 1] };
			else cout << "Usage: -d <drawparam path>" << endl;
		}
	}



	cout << "Initializing VTK..." << endl;

	//lisää aikasarjatuen?
	auto *sdp = vtkStreamingDemandDrivenPipeline::New();
	vtkAlgorithm::SetDefaultExecutivePrototype(sdp);
	sdp->Delete();

	auto renWin = vtkSmartPointer<vtkRenderWindow>::New();

	renWin->SetSize(800, 800);

	vtkWin32OutputWindow::SafeDownCast(vtkOutputWindow::GetInstance())->SendToStdErrOn();

	auto iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);



	auto meta = nbsMetadata();
	meta.init(surfFile);


	auto style = vtkSmartPointer<fmiVis::VisualizationInteractor2D>::New();


	iren->SetInteractorStyle(style);

	size_t sizeX = 3;
	size_t sizeY = 2;

	auto vm = fmiVis::ViewportManagerTimegrid{ sizeX,sizeY };

	fmiVis::ViewportFactory::MakeTimeGridView(sizeX, sizeY,surfFile, drawParamPath, meta, vm, iren, renWin, style);

	style->GetImpl().setVM(&vm);

	iren->Initialize();
	iren->Start();



	return EXIT_SUCCESS;
}


