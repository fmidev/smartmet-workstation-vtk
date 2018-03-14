#include "Interactor.h"


#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <vtkImageData.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include "ViewportManager.h"
#include "TimeAnimator.h"

void fmiVis::VisualizationInteractorImpl::OnKeyRelease(std::string &s,bool ctrl,vtkRenderer* vp)
{


	double val = ta->GetTime();

	if (s.find("space") == 0) {
		ta->ToggleAnim();
	}
	else
		if (s.find("Right") == 0) {

			ta->AnimateStep(false);
		}
		else if (s.find("Left") == 0) {

			ta->AnimateStep(false, -1);
		}
		else if (s.find("Up") == 0) {
			ta->DecreaseDelay();
		}
		else if (s.find("Down") == 0) {
			ta->IncreaseDelay();
		}
		else
		{


			int vid;

			bool hasF = s[0] == 'F';

			try
			{
				if (hasF)
					s.erase(0, 1);
				vid = stoi(s);
			}
			catch (std::invalid_argument e) {
				vid = -1;
			}

			if (vid == 0) vid = 10;

			if (hasF) vid += 10;

			vid--;

			try {
				vm->ViewportNumKey(vm->whichViewport(vp), vid, ctrl);
			}
			catch (std::invalid_argument e) {}

			}


	// cout << "Pressed key " << (ctrl ? "ctrl+" : std::string()) << ", sym " << s << endl;
}