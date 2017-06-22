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

	//false = streamline
	bool mode;

	vtkAssignAttribute *assign;

	vtkGlyph3D *glypher;

	vtkExtractSelectedIds *extract;
	vtkStreamTracer *streamer;
	vtkRibbonFilter  *ribbon;

	vtkPolyDataMapper *map;

	vtkActor *act;

	void ModeStreamline();
	void ModeGlyph();

public:
	static const int PARAM_WINDVEC = 30000;

	ParamVisualizerWindVec2D(const std::string &file, nbsMetadata &m);
	~ParamVisualizerWindVec2D();

	virtual inline void ToggleMode() {

		if (mode) {
			ModeStreamline();
		}
		else {
			ModeGlyph();
		}

		mode = !mode;
	}

};
#endif // ParamVisualizerWindVec2D_h__