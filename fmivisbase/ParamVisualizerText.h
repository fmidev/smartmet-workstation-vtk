#ifndef ParamVisualizerText_h__
#define ParamVisualizerText_h__

#include <vtkSmartPointer.h>


#include "ParamVisualizerBase.h"


class vtkVectorText;
class vtkPolyDataMapper;
class vtkActor;
class vtkAppendPolyData;
class vtkTransformPolyDataFilter;

class ParamVisualizerText : public ParamVisualizerBase {

	vtkVectorText *vt;
	vtkTransformPolyDataFilter *tf;
	vtkAppendPolyData *ap;

	vtkPolyDataMapper *map;
	vtkActor *act;

public:
	ParamVisualizerText(const std::string &file, nbsMetadata &m, int param);
	~ParamVisualizerText();

	void UpdateTimeStep(double t) override;

};

#endif // ParamVisualizerText_h__
