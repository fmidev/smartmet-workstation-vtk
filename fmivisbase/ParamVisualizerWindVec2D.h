#ifndef ParamVisualizerWindVec2D_h__
#define ParamVisualizerWindVec2D_h__

#include <vtkSmartPointer.h>

#include "ParamVisualizerBase.h"

struct nbsMetadata;
class vtkAlgorithmOutput;
class vtkGlyph3D;
class vtkExtractSelectedIds;
class vtkStreamTracer;
class vtkRibbonFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkPolyDataAlgorithm;
class vtkAssignAttribute;
class NFmiDataIdent;

class ParamVisualizerWindVec2D : public ParamVisualizerBase {

	enum MODE 
	{ 
		mode_stream = 0,
		mode_barb = 1,
		mode_arrow = 2,

		mode_count
	} mode;

	vtkSmartPointer<vtkAssignAttribute> assign;

	vtkSmartPointer<vtkGlyph3D> glypher;

	vtkSmartPointer<vtkExtractSelectedIds> extract;
	vtkSmartPointer<vtkStreamTracer> streamer;
	vtkSmartPointer<vtkRibbonFilter> ribbon;

	vtkSmartPointer<vtkPolyDataMapper> map;

	vtkSmartPointer<vtkActor> act;

	void ModeStreamline();
	void ModeBarb();
	void ModeArrow();

public:

	ParamVisualizerWindVec2D(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac);

	virtual void UpdateTimeStep(double t) override;

	virtual inline void ToggleMode() {

		if (mode==mode_stream) {
			ModeBarb();
		}
		else if(mode==mode_barb){
			ModeArrow(); 
		}
		else if (mode == mode_arrow)
			ModeStreamline();

	}

};
#endif // ParamVisualizerWindVec2D_h__