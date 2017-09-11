#ifndef ParamVisualizerText_h__
#define ParamVisualizerText_h__

#include <vtkSmartPointer.h>


#include "ParamVisualizerBase.h"
#include "TextImageLayer.h"

class vtkPolyDataMapper;
class vtkActor;
class vtkAppendPolyData;
class vtkTransformPolyDataFilter;
class vtkTexture;


class ParamVisualizerText : public ParamVisualizerBase {

	TextImageLayer tl;
	vtkSmartPointer<vtkTransformPolyDataFilter> tf;
	vtkSmartPointer<vtkAppendPolyData> ap;

	vtkSmartPointer<vtkTexture> texture;
	vtkSmartPointer<vtkPolyDataMapper> map;
	vtkSmartPointer<vtkActor> act;

public:
	ParamVisualizerText(const std::string &file, nbsMetadata &m, int param);


	void UpdateTimeStep(double t) override;

};

#endif // ParamVisualizerText_h__
