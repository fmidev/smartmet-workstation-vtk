#ifndef ParamVisualizerWindVec_h
#define ParamVisualizerWindVec_h

#include <vtkSmartPointer.h>

#include "ParamVisualizerBase.h"

struct nbsMetadata;
class vtkAlgorithmOutput;
class vtkGlyph3D;
class vtkExtractSelectedIds;
class vtkStreamTracer;
class vtkTubeFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkPolyDataAlgorithm;
class vtkProbeFilter;
class vtkAssignAttribute;
class NFmiDataIdent;

class ParamVisualizerWindVec : public ParamVisualizerBase {

	//false = streamline
	bool mode;

	vtkSmartPointer<vtkAlgorithmOutput> seedData;
	vtkSmartPointer<vtkAssignAttribute> assign;

	vtkSmartPointer<vtkProbeFilter> probe;

	vtkSmartPointer<vtkGlyph3D> glypher;

	vtkSmartPointer<vtkExtractSelectedIds> extract;
	vtkSmartPointer<vtkStreamTracer> streamer;
	vtkSmartPointer<vtkTubeFilter> tuber;

	vtkSmartPointer<vtkPolyDataMapper> map;

	vtkSmartPointer<vtkActor> act;

	void ModeStreamline();
	void ModeGlyph();

public:

	ParamVisualizerWindVec(const std::string &file, nbsMetadata &m, NFmiDataIdent &param, NFmiDrawParamFactory* fac, vtkAlgorithmOutput* seedData);

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

#endif // ParamVisualizerWindVec_h
