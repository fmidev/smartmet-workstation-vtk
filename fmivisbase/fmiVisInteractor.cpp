#include "fmiVisInteractor.h"


#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <vtkImageData.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include "VisualizerManager.h"
#include "TimeAnimator.h"

void fmiVisInteractor::OnKeyRelease()
{
	vtkRenderWindowInteractor *rwi = this->Interactor;

	bool ctrl = rwi->GetControlKey();
	auto s = std::string(rwi->GetKeySym());

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


			if (vid >= 0 && vid < vm->GetVisNum()) {
				if (ctrl) {

					vm->ToggleMode(vid);
					visualizerTexts->at(vid)->GetTextProperty()->SetItalic(!visualizerTexts->at(vid)->GetTextProperty()->GetItalic());
				}
				else {

					if (vm->IsEnabled(vid)) {
						visualizerTexts->at(vid)->GetTextProperty()->SetColor(0.2, 0.2, 0.2);
					}
					else {
						visualizerTexts->at(vid)->GetTextProperty()->SetColor(0.8, 0.8, 0.8);
					}

					vm->ToggleVis(vid);
				}
			}
		}

	rwi->Render();

	// cout << "Pressed key " << (ctrl ? "ctrl+" : std::string()) << ", sym " << s << endl;
}

void fmiVisInteractor::OnChar()
{
	vtkRenderWindowInteractor *rwi = this->Interactor;
	char k = rwi->GetKeyCode();


	if (k != 'l' && k != 'f' && k != 's' && (k < 0 && k>9)) //disable latlon-sphere, fly-to, stereo
		this->Superclass::OnChar();
}
