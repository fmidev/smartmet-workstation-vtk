#ifndef ParamVisualizerArrayText_h__
#define ParamVisualizerArrayText_h__

#include <vtkSmartPointer.h>


#include "ParamVisualizerBase.h"

#include <vtkSmartPointer.h>

class vtkTextActor3D;
class vtkPropAssembly;
class vtkScalarsToColors;

namespace fmiVis {

	//poorly performing array-of-actors text visualizer that can customize each point of text individually
	class ParamVisualizerArrayText : public ParamVisualizerBase {

		std::vector<vtkTextActor3D*> acts;

		vtkSmartPointer<vtkPropAssembly> assembly;

		vtkSmartPointer<vtkScalarsToColors> colF;

	public:
		ParamVisualizerArrayText(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac, vtkSmartPointer<vtkScalarsToColors> col = nullptr);
		~ParamVisualizerArrayText();

		void UpdateTimeStep(double t) override;

	};


}

#endif // ParamVisualizerArrayText_h__
