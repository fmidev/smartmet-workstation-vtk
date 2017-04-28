#ifndef FMIVISINTERACTOR_H
#define FMIVISINTERACTOR_H

#include <vtkInteractorStyleTerrain.h>
#include <vtkCamera.h>
#include <vtkCallbackCommand.h>
#include <vtkMath.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include "VisualizerManager.h"
#include "vtkImageData.h"
#include "vtkTextActor.h"

#include "TimeAnimator.h"



class fmiVisInteractor: public vtkInteractorStyleTerrain {
public:
	static inline fmiVisInteractor *New() {
		return new fmiVisInteractor();
	};


	std::vector<vtkSmartPointer<vtkTextActor>> *visualizerTexts;

	VisualizerManager *vm;

	inline void setVM(VisualizerManager *ta) {
		this->vm = vm;
	}

	TimeAnimator *ta;

	inline void setTA(TimeAnimator *ta) {
		this->ta = ta;
	}

	inline void setVisTexts(std::vector<vtkSmartPointer<vtkTextActor> > *visualizerTexts) {
		this->visualizerTexts = visualizerTexts;
	}


	virtual void OnKeyRelease() {
		vtkRenderWindowInteractor *rwi = this->Interactor;

		bool ctrl = rwi->GetControlKey();
		auto s = std::string( rwi->GetKeySym() );

		double val = ta->GetTime();

		if(s.find("space") == 0 ) {
			ta->ToggleAnim();
		} else
		if (s.find("Right") == 0) {

			ta->AnimateStep(false);
		}
		else if (s.find("Left") == 0) {

			ta->AnimateStep(false, -1);
		}
		else if (s.find("Up") ) {
			ta->DecreaseDelay();
		}
		else if (s.find("Down")) {
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


		cout << "Pressed key " << (ctrl ? "ctrl+" : std::string()) << ", sym "<<s<< endl;
	}

	virtual void OnChar()
	{
		vtkRenderWindowInteractor *rwi = this->Interactor;
		char k = rwi->GetKeyCode();
		 

		if(k != 'l' && k!= 'f' && k!='s' && (k<0 && k>9) ) //disable latlon-sphere, fly-to, stereo
			this->Superclass::OnChar();
	}
	protected:
		fmiVisInteractor() {
		}
};


#endif /* FMIVISINTERACTOR_H */