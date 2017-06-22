#ifndef FMIVISINTERACTOR_H
#define FMIVISINTERACTOR_H
 
#include <vector>

#include <vtkSmartPointer.h>

#include <vtkInteractorObserver.h>
#include <vtkInteractorStyleTerrain.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindowInteractor.h>

class vtkTextActor;

class VisualizerManager;
class TimeAnimator;

class fmiVisInteractorBase {
public:

	std::vector<vtkSmartPointer<vtkTextActor>> *visualizerTexts;


	VisualizerManager *vm;

	TimeAnimator *ta;



	void OnKeyRelease(std::string &s, bool ctrl);


};

class fmiVisInteractor : public vtkInteractorStyleTerrain {
public:
	static inline fmiVisInteractor *New() {
		return new fmiVisInteractor();
	};

	inline void setVM(VisualizerManager *vm) {
		b.vm = vm;
	}

	inline void setTA(TimeAnimator *ta) {
		b.ta = ta;
	}

	inline void setVisTexts(std::vector<vtkSmartPointer<vtkTextActor> > *visualizerTexts) {
		b.visualizerTexts = visualizerTexts;
	}

	inline void OnKeyRelease() {
		vtkRenderWindowInteractor *rwi = this->Interactor;

		auto s = std::string(rwi->GetKeySym());
		bool ctrl = rwi->GetControlKey();

		b.OnKeyRelease(s, ctrl);

		rwi->Render();
	}


	inline void OnChar() {

		vtkRenderWindowInteractor *rwi = this->Interactor;
		char k = rwi->GetKeyCode();


		if (k != 'l' && k != 'f' && k != 's' && (k < 0 && k>9)) //disable latlon-sphere, fly-to, stereo
			Superclass::OnChar();
	}

protected:
	fmiVisInteractorBase b;
	fmiVisInteractor() { }
};

class fmiVisInteractor2D :  public vtkInteractorStyleImage {
public:
	static inline fmiVisInteractor2D *New() {
		return new fmiVisInteractor2D();
	};

	inline void setVM(VisualizerManager *vm) {
		b.vm = vm;
	}

	inline void setTA(TimeAnimator *ta) {
		b.ta = ta;
	}

	inline void setVisTexts(std::vector<vtkSmartPointer<vtkTextActor> > *visualizerTexts) {
		b.visualizerTexts = visualizerTexts;
	}

	virtual void OnKeyRelease() {
		vtkRenderWindowInteractor *rwi = this->Interactor;

		auto s = std::string(rwi->GetKeySym());
		bool ctrl = rwi->GetControlKey();

		b.OnKeyRelease(s, ctrl);

		rwi->Render();
	}

	inline void OnChar() {

		vtkRenderWindowInteractor *rwi = this->Interactor;
		char k = rwi->GetKeyCode();


		if (k != 'l' && k != 'f' && k != 's' && (k < 0 && k>9)) //disable latlon-sphere, fly-to, stereo
			Superclass::OnChar();
	}

protected:
	fmiVisInteractorBase b;
	fmiVisInteractor2D() { }
};


#endif /* FMIVISINTERACTOR_H */