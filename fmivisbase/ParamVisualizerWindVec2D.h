#ifndef ParamVisualizerWindVec2D_h__
#define ParamVisualizerWindVec2D_h__

#include <vtkSmartPointer.h>

#include "ParamVisualizerBase.h"

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

class vtkEvenlySpacedStreamlines2D;

namespace fmiVis {

	struct nbsMetadata;



	//planar specialization of windvectors, duplicates code. includes arrows as glyphs
	//vtkEvenlySpacedStreamlines is bugged, see https://gitlab.kitware.com/vtk/vtk/issues/17380 and weep
	//manually patched by changing PointIsWithinBounds()'s delta argument to a zero vector in vtkEvenlySpacedStreamlines2D::RequestData()
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
		vtkSmartPointer<vtkEvenlySpacedStreamlines2D> spacedStreamer;
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

			if (mode == mode_stream) {
				ModeBarb();
			}
			else if (mode == mode_barb) {
				ModeArrow();
			}
			else if (mode == mode_arrow)
				ModeStreamline();

		}

	};

}
#endif // ParamVisualizerWindVec2D_h__