#ifndef PARAMVISUALIZERBASE_H
#define PARAMVISUALIZERBASE_H

#include <memory>
#include <list>
#include <vtkSmartPointer.h>

#include <vtkProp.h>

#include "nbsMetadata.h"

class vtkAbstractMapper;
class vtkAlgorithm;
class vtkPlanes;
class vtkScalarsToColors;
class vtkRenderer;
class NFmiDrawParamFactory;
class NFmiDataIdent;

namespace fmiVis {

	//Contains basic interface and functionality common to visualizers, like communicating with the visualizermanager
	class ParamVisualizerBase {
	private:
		vtkAbstractMapper *activeMapper;
		vtkProp *prop;
		bool crop;
		bool enabled;


	protected:

		vtkRenderer * ren;


		NFmiDrawParamFactory* drawParamFac = nullptr;

		NFmiDataIdent &paramIdent;

		const nbsMetadata &meta;

		std::list<vtkAlgorithm* > filters;

		vtkAlgorithm* nbs;

		//choose the actual output of this visualizer through this - some visualizers require different types of them
		void SetActiveMapper(vtkAbstractMapper *m);
		void SetProp(vtkProp *p);
	public:
		int param;

		ParamVisualizerBase(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac);
		ParamVisualizerBase(vtkAlgorithm *nbs, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac);
		virtual ~ParamVisualizerBase();



		void CropMapper(vtkPlanes* p);

		//controls the actual cropping, CropMapper is called quite often
		inline void SetCropping(bool c) {
			crop = c;
		}


		inline void SetRenderer(vtkRenderer* r) {
			ren = r;
		}

		virtual void UpdateTimeStep(double t);

		virtual void UpdateNBS(double t);

		inline void EnableActor() {
			enabled = true;

			if (prop) prop->SetVisibility(true);
		}

		inline void DisableActor() {
			enabled = false;

			if (prop) prop->SetVisibility(false);

		}

		virtual vtkScalarsToColors  * getColor() { return nullptr; }
		virtual double * getRange() { return nullptr; }

		inline bool IsEnabled() {
			return enabled;
		}

		inline vtkProp* GetProp() {
			return prop;
		}

		inline void SetDrawParamFac(NFmiDrawParamFactory *p) {
			drawParamFac = p;
		}

		//subclasses are to cycle their drawing modes when called
		//disconnect unused filters to prevent them from being updated
		virtual void ToggleMode() {};

		//only implemented in surface visualizer at the moment
		virtual void ReloadOptions() {};

	};

}

#endif /* PARAMVISUALIZERBASE_H */