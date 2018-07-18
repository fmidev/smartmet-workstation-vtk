#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE

#include <math.h>

#include <list>

#include <vtkVersion.h>
#include <vtkSmartPointer.h>

#include <vtkOutputWindow.h>
#include <vtkAlgorithm.h>
#include <vtkWin32OutputWindow.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>

#include <vtkStreamingDemandDrivenPipeline.h>

#include "Interactor.h"
#include "VisualizerManager.h"

#include "TimeAnimator.h"


#include "VolVis.h"
#include "ViewportManagerTimegrid.h"

static std::string *newBaseFile;

using namespace std::string_literals;

int main(int argc, char *argv[])
{



	cout << "Initializing interface..." << endl;

	fmiVis::InitConfig();

	std::string file = "D:/3D-dataa/201703210327_hirlam_skandinavia_mallipinta.sqd"s;

	std::string surfFile;

	std::string drawParamPath = "D:/3D-dataa/DrawParams";

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0) {
			if (i < argc - 1)
				file = std::string{ argv[i + 1] };
			else cout << "Usage: -f <data file>" << endl;
		}
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


	auto ren = vtkSmartPointer<vtkRenderer>::New();


	ren->SetBackground(0.4, 0.4, 0.8);


	auto renWin = vtkSmartPointer<vtkRenderWindow>::New();
	renWin->SetSize(800, 800);

	renWin->AddRenderer(ren);

	auto iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	auto style = vtkSmartPointer<fmiVis::VisualizationInteractor>::New();

	iren->SetInteractorStyle(style);


	auto meta = nbsMetadata();
	meta.init(file);


	auto vm = fmiVis::ViewportManager{ };


	auto widgets = fmiVis::Make3DView(file, surfFile, drawParamPath, meta, vm, iren.Get(), renWin.Get(), style.Get());

	style->GetImpl().setVM(&vm);

	ren->ResetCamera();
	ren->Render();

	iren->Initialize();
	iren->Start();

	return EXIT_SUCCESS;
}