#ifndef FMIVISINTERACTOR_H
#define FMIVISINTERACTOR_H

#include <vector>

#include <vtkSmartPointer.h>

#include <vtkInteractorStyleTerrain.h>

class vtkTextActor;

class VisualizerManager;
class TimeAnimator;

class fmiVisInteractor: public vtkInteractorStyleTerrain {
public:
	static inline fmiVisInteractor *New() {
		return new fmiVisInteractor();
	};


	std::vector<vtkSmartPointer<vtkTextActor>> *visualizerTexts;

	VisualizerManager *vm;

	inline void setVM(VisualizerManager *vm) {
		this->vm = vm;
	}

	TimeAnimator *ta;

	inline void setTA(TimeAnimator *ta) {
		this->ta = ta;
	}

	inline void setVisTexts(std::vector<vtkSmartPointer<vtkTextActor> > *visualizerTexts) {
		this->visualizerTexts = visualizerTexts;
	}


	virtual void OnKeyRelease();

	virtual void OnChar();
	protected:
		fmiVisInteractor() {
		}
};


#endif /* FMIVISINTERACTOR_H */