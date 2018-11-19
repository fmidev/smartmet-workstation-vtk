#ifndef ParamVisualizerPolyText_h__
#define ParamVisualizerPolyText_h__

#include <vtkSmartPointer.h>


#include "ParamVisualizerBase.h"


class vtkVectorText;
class vtkPolyDataMapper;
class vtkActor;
class vtkAppendPolyData;
class vtkTransformPolyDataFilter;
class NFmiDrawParamFactory;
class NFmiDataIdent;

namespace fmiVis {

	//displays modifiable text as polygonal meshes...
	class ParamVisualizerPolyText : public ParamVisualizerBase {

		vtkSmartPointer<vtkVectorText> vt;
		vtkSmartPointer<vtkTransformPolyDataFilter> tf;
		vtkSmartPointer<vtkAppendPolyData> ap;

		vtkSmartPointer<vtkPolyDataMapper> map;
		vtkSmartPointer<vtkActor> act;

	public:
		ParamVisualizerPolyText(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac);

		void UpdateTimeStep(double t) override;

	};

}
#endif // ParamVisualizerPolyText_h__
