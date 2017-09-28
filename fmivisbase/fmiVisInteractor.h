#ifndef FMIVISINTERACTOR_H
#define FMIVISINTERACTOR_H
 
#include <vector>
#include <memory>

#include <vtkSmartPointer.h>

#include <vtkInteractorObserver.h>
#include <vtkInteractorStyleTerrain.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

#include "ViewportManager.h"

class vtkTextActor;

class VisualizerManager;
class TimeAnimator;
class fmiVis::ViewportManager;

class fmiVisInteractorBase {
public:

	std::vector< vtkSmartPointer<vtkTextActor> > *visualizerTexts;


	fmiVis::ViewportManager *vm;

	std::unique_ptr<TimeAnimator> ta;



	void OnKeyRelease(std::string &s, bool ctrl,vtkRenderer *vp = nullptr);


};

class fmiVisInteractor : public vtkInteractorStyleTerrain {
public:
	static inline fmiVisInteractor *New() {
		return new fmiVisInteractor();
	};

	inline void setVM(fmiVis::ViewportManager *vm) {
		b.vm = vm;

	}

	inline void setTA(std::unique_ptr<TimeAnimator> &&ta) {
		b.ta = std::move(ta);
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

	inline void setVM(fmiVis::ViewportManager *vm) {
		b.vm = vm;
	}

	inline void setTA(std::unique_ptr<TimeAnimator> &&ta) {
		b.ta = std::move(ta);
	}

	inline void setVisTexts(std::vector<vtkSmartPointer<vtkTextActor> > *visualizerTexts) {
		b.visualizerTexts = visualizerTexts;
	}

	virtual void OnKeyRelease() {
		vtkRenderWindowInteractor *rwi = this->Interactor;

		auto s = std::string(rwi->GetKeySym());
		bool ctrl = rwi->GetControlKey();


		int x = this->Interactor->GetEventPosition()[0];
		int y = this->Interactor->GetEventPosition()[1];
		this->FindPokedRenderer(x, y);

		b.OnKeyRelease(s, ctrl,this->GetCurrentRenderer());

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