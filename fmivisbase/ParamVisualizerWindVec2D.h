#ifndef ParamVisualizerWindVec2D_h__
#define ParamVisualizerWindVec2D_h__


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

class ParamVisualizerWindVec2D : public ParamVisualizerBase {

	enum MODE 
	{ 
		mode_stream = 0,
		mode_barb = 1,
		mode_arrow = 2,

		mode_count
	} mode;

	vtkAssignAttribute *assign;

	vtkGlyph3D *glypher;

	vtkExtractSelectedIds *extract;
	vtkStreamTracer *streamer;
	vtkRibbonFilter  *ribbon;

	vtkPolyDataMapper *map;

	vtkActor *act;

	void ModeStreamline();
	void ModeBarb();
	void ModeArrow();

public:
	static const int PARAM_WINDVEC = 30000;

	ParamVisualizerWindVec2D(const std::string &file, nbsMetadata &m);
	~ParamVisualizerWindVec2D();

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