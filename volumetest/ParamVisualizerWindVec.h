#ifndef ParamVisualizerWindVec_h
#define ParamVisualizerWindVec_h

#include "ParamVisualizerBase.h"

struct nbsMetadata;
class vtkAlgorithmOutput;
class vtkGlyph3D;
class vtkCleanPolyData;
class vtkStreamTracer;
class vtkTubeFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkPolyDataAlgorithm;

class ParamVisualizerWindVec : public ParamVisualizerBase {


	vtkAlgorithmOutput *seedData;

	//false = streamline
	bool mode; 

	vtkGlyph3D *glypher;

	vtkCleanPolyData *cleaner;
	vtkStreamTracer *streamer;
	vtkTubeFilter *tuber;

	vtkPolyDataMapper *map;

	vtkActor *act;

	void ModeStreamline();
	void ModeGlyph();

public:
	static const int PARAM_WINDVEC = 30000;

	ParamVisualizerWindVec(const std::string &file, nbsMetadata &m, vtkAlgorithmOutput* seedData);
	~ParamVisualizerWindVec();

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
