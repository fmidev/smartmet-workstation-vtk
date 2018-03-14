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


#include "TimeAnimator.h"
#include "ViewportManager.h"

class vtkTextActor;

class VisualizerManager;


namespace fmiVis {

	class TimeAnimator;
	class ViewportManager;

	class VisualizationInteractorImpl {
	public:

		std::vector< vtkSmartPointer<vtkTextActor> > *visualizerTexts;


		ViewportManager *vm;

		std::unique_ptr<TimeAnimator> ta;



		void OnKeyRelease(std::string &s, bool ctrl, vtkRenderer *vp = nullptr);


		inline void setVM(ViewportManager *vm) {
			this->vm = vm;

		}

		inline void setTA(std::unique_ptr<TimeAnimator> &&ta) {
			this->ta = std::move(ta);
		}
	};

	class VisualizationInteractor : public vtkInteractorStyleTerrain {
	public:
		static inline VisualizationInteractor *New() {
			return new VisualizationInteractor();
		};

		inline VisualizationInteractorImpl& GetImpl() {
			return impl;
		}



		inline void setVisTexts(std::vector<vtkSmartPointer<vtkTextActor> > *visualizerTexts) {
			impl.visualizerTexts = visualizerTexts;
		}

		inline void OnKeyRelease() {
			vtkRenderWindowInteractor *rwi = this->Interactor;

			auto s = std::string(rwi->GetKeySym());
			bool ctrl = rwi->GetControlKey();

			this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(
				this->Interactor->GetLastEventPosition()[0],
				this->Interactor->GetLastEventPosition()[1]));

			impl.OnKeyRelease(s, ctrl, this->GetCurrentRenderer());

			rwi->Render();
		}


		inline void OnChar() {

			vtkRenderWindowInteractor *rwi = this->Interactor;
			char k = rwi->GetKeyCode();


			if (k != 'l' && k != 'f' && k != 's' && (k < 0 && k>9)) //disable latlon-sphere, fly-to, stereo
				Superclass::OnChar();
		}

	protected:
		VisualizationInteractorImpl impl;
		VisualizationInteractor() { }
	};

	class VisualizationInteractor2D : public vtkInteractorStyleImage {
	public:
		static inline VisualizationInteractor2D *New() {
			return new VisualizationInteractor2D();
		};


		inline VisualizationInteractorImpl& GetImpl() {
			return impl;
		}


		inline void setVisTexts(std::vector<vtkSmartPointer<vtkTextActor> > *visualizerTexts) {
			impl.visualizerTexts = visualizerTexts;
		}

		virtual void OnKeyRelease() {
			vtkRenderWindowInteractor *rwi = this->Interactor;

			auto s = std::string(rwi->GetKeySym());
			bool ctrl = rwi->GetControlKey();


			int x = this->Interactor->GetEventPosition()[0];
			int y = this->Interactor->GetEventPosition()[1];
			this->FindPokedRenderer(x, y);

			impl.OnKeyRelease(s, ctrl, this->GetCurrentRenderer());

			rwi->Render();
		}

		inline void OnChar() {

			vtkRenderWindowInteractor *rwi = this->Interactor;
			char k = rwi->GetKeyCode();


			if (k != 'l' && k != 'f' && k != 's' && (k < 0 && k>9)) //disable latlon-sphere, fly-to, stereo
				Superclass::OnChar();
		}


	protected:
		VisualizationInteractorImpl impl;
		VisualizationInteractor2D() { }
	};
}


#endif /* FMIVISINTERACTOR_H */