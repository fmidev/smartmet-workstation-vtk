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
	vtkTransformPolyDataFilter *tf;
	vtkAppendPolyData *ap;

	vtkTexture *texture;
	vtkPolyDataMapper *map;
	vtkActor *act;

public:
	ParamVisualizerText(const std::string &file, nbsMetadata &m, int param);
	~ParamVisualizerText();

	void UpdateTimeStep(double t) override;

};

#endif // ParamVisualizerText_h__