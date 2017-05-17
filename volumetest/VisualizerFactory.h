#ifndef VISUALIZERFACTORY_H
#define VISUALIZERFACTORY_H

#include <memory>
#include <map>

#include <vtkSmartPointer.h>


#include "ParamVisualizerBase.h"

class vtkColorTransferFunction;
class vtkPiecewiseFunction;
class ContourLabeler;
class vtkAlgorithmOutput;

struct nbsMetadata;


class ParamVisualizer2D;
class ParamVisualizer3D;


struct Vis3DParams {
	int param;
	vtkSmartPointer<vtkColorTransferFunction> color;
	vtkSmartPointer<vtkPiecewiseFunction> opacity;
	double contourColor[3];
	double contourThreshold;
	double contourOpacity;
};

struct Vis2DParams {
	int param;
	vtkSmartPointer<vtkColorTransferFunction> contourColors;
	double range[2];
	int numContours;
};


class VisualizerFactory {
public:

	static vtkSmartPointer<vtkColorTransferFunction> blueToRedColor(double min, double max);
	static vtkSmartPointer<vtkColorTransferFunction> greenToRedColor(double min, double max);

	static vtkSmartPointer<vtkColorTransferFunction> blueToRedColor(double * r);
	static vtkSmartPointer<vtkColorTransferFunction> greenToRedColor(double * r);

	static vtkSmartPointer<vtkPiecewiseFunction> opacityFunction(double min, double max, double threshold,
																			double maxAlpha=0.2, double minAlpha=0.0);

	static std::unique_ptr<ParamVisualizerBase> make3DVisualizer(const std::string &file, nbsMetadata &meta,
		int param, vtkSmartPointer<vtkColorTransferFunction> color, vtkSmartPointer<vtkPiecewiseFunction> opacity,
		double contourColor[3], double contourThreshold, double contourOpacity = 1.0f);


	static std::unique_ptr<ParamVisualizerBase> make3DVisualizer(const std::string &file, nbsMetadata &meta,Vis3DParams &p);
	static std::unique_ptr<ParamVisualizerBase> make3DVisualizer(const std::string &file, nbsMetadata &meta, int param);

	static std::unique_ptr<ParamVisualizerBase> make2DVisualizer(const std::string &file, nbsMetadata &meta,
		vtkAlgorithmOutput* probingData, int param,  vtkSmartPointer<vtkColorTransferFunction> contourColors,
		ContourLabeler & labeler, double range[2], int numContours = 10);

	static std::unique_ptr<ParamVisualizerBase> make2DVisualizer(const std::string &file, nbsMetadata &meta,
		vtkAlgorithmOutput* probingData, ContourLabeler & labeler, Vis2DParams &p);

	static std::unique_ptr<ParamVisualizerBase> make2DVisualizer(const std::string &file, nbsMetadata &meta, vtkAlgorithmOutput* probingData, ContourLabeler & labeler, int param);

	static std::map<int, Vis3DParams> config3D;
	static std::map<int, Vis2DParams> config2D;

	static void init();

};



#endif /* VISUALIZERFACTORY_H */
