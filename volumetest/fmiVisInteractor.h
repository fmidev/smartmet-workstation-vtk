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
#include "vtkSliderRepresentation2D.h"
#include "vtkAnimationScene.h"



class fmiVisInteractor: public vtkInteractorStyleTerrain {
public:
	static inline fmiVisInteractor *New() {
		return new fmiVisInteractor();
	};

	VisualizerManager *vm;
	
	vtkSliderRepresentation2D *timeSlider;


	std::vector<vtkSmartPointer<vtkTextActor>> *visualizerTexts;

	metaData *meta;

	bool *anim;

	double animStart;

	inline void setVM(VisualizerManager *vm) {
		this->vm = vm;
	}

	inline void setVisTexts(std::vector<vtkSmartPointer<vtkTextActor> > *visualizerTexts) {
		this->visualizerTexts = visualizerTexts;
	}

	inline void setSlider(vtkSliderRepresentation2D *slider) {
		timeSlider = slider;
	}

	inline void setMeta(metaData *m) {
		meta = m;
	}
	inline void setAnim(bool *a) {
		anim = a;
	}

	virtual void OnKeyRelease() {
		vtkRenderWindowInteractor *rwi = this->Interactor;

		bool ctrl = rwi->GetControlKey();
		auto s = std::string( rwi->GetKeySym() );

		bool timeChanged = false;

		double val = timeSlider->GetValue();

		if(s.find("space") == 0 ) {
			if ( *anim ) {
				*anim = false;
			}
			else {

				*anim = true;
			}
		} else
		if (s.find("Right") == 0) {

			timeChanged = true;
			val += 1;

			if (val > timeSlider->GetMaximumValue()) val = timeSlider->GetMaximumValue();
		}
		else if (s.find("Left") == 0) {

			timeChanged = true;
			val -= 1;

			if (val < timeSlider->GetMinimumValue()) val = timeSlider->GetMinimumValue();
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

		if (timeChanged) {
			time_t time = meta->timeStepToEpoch(val);
			timeSlider->SetTitleText(metaData::getTimeString(time).c_str());

			timeSlider->SetValue(val);
			vm->UpdateTimeStep(val);
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
			animStart = 0;
		}
};


#endif /* FMIVISINTERACTOR_H */