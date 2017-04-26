#ifndef VISUALIZERFACTORY_H
#define VISUALIZERFACTORY_H

#include <vtkSmartPointer.h>

#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

#include "ParamVisualizer3D.h"
#include "ParamVisualizer2D.h"


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

	inline static vtkSmartPointer<vtkColorTransferFunction> blueToRedColor(double min, double max) {
		auto f = vtkSmartPointer<vtkColorTransferFunction>::New();
		f->AddRGBPoint(min, 0.0, 0.0, 1.0);
		f->AddRGBPoint((min+max)/2, 0.6, 0, 0.6);
		f->AddRGBPoint(max, 1.0, 0, 0.0);
		return f;
	}
	inline static vtkSmartPointer<vtkColorTransferFunction> greenToRedColor(double min, double max) {
		auto f = vtkSmartPointer<vtkColorTransferFunction>::New();
		f->AddRGBPoint(min, 0.0, 0.5, 0.0);
		f->AddRGBPoint((min + max) / 3, 0, 1, 0);
		f->AddRGBPoint(2*(min + max) / 3, 0.7, 0.7, 0);
		f->AddRGBPoint(max, 1.0, 0, 0.0);
		return f;
	}
	inline static vtkSmartPointer<vtkPiecewiseFunction> opacityFunction(double min, double max, double threshold,
																			double maxAlpha=0.2, double minAlpha=0.0) {
		auto f = vtkSmartPointer<vtkPiecewiseFunction>::New();
		f->AddPoint(min, 0.0f);
		f->AddPoint(threshold, minAlpha);
		f->AddPoint(max, maxAlpha);
		return f;
	}

	inline static std::unique_ptr<ParamVisualizerBase> make3DVisualizer(const std::string &file, metaData &meta,
		int param, vtkSmartPointer<vtkColorTransferFunction> color, vtkSmartPointer<vtkPiecewiseFunction> opacity,
		double contourColor[3], double contourThreshold, double contourOpacity = 1.0f) {
		return std::make_unique<ParamVisualizer3D>(file, meta, param, color, opacity, contourThreshold, contourColor, contourOpacity);
	}
	inline static std::unique_ptr<ParamVisualizerBase> make3DVisualizer(const std::string &file, metaData &meta,Vis3DParams &p) {
		return make3DVisualizer(file, meta, p.param, p.color, p.opacity, p.contourColor, p.contourThreshold, p.contourOpacity);
	}
	inline static std::unique_ptr<ParamVisualizerBase> make3DVisualizer(const std::string &file, metaData &meta, int param) {
		if(config3D.find(param)!=config3D.end() )
			return make3DVisualizer(file, meta, config3D.find(param)->second);
		return nullptr;
	}

	inline static std::unique_ptr<ParamVisualizerBase> make2DVisualizer(const std::string &file, metaData &meta,
		vtkAlgorithmOutput* probingData,
		int param,  vtkSmartPointer<vtkColorTransferFunction> contourColors, std::shared_ptr<ContourLabeler> labeler, double range[2], int numContours = 10) {
		return std::make_unique<ParamVisualizer2D>(file, meta,param,probingData, contourColors,labeler,range,numContours);
	}
	inline static std::unique_ptr<ParamVisualizerBase> make2DVisualizer(const std::string &file, metaData &meta,
		vtkAlgorithmOutput* probingData, std::shared_ptr<ContourLabeler> labeler, Vis2DParams &p) {
		return make2DVisualizer(file, meta,probingData,p.param, p.contourColors, labeler, p.range, p.numContours);
	}

	inline static std::unique_ptr<ParamVisualizerBase> make2DVisualizer(const std::string &file, metaData &meta, vtkAlgorithmOutput* probingData, std::shared_ptr<ContourLabeler> labeler, int param) {
		if (config2D.find(param) != config2D.end())
			return make2DVisualizer(file, meta, probingData, labeler, config2D.find(param)->second);
		else
			return nullptr;
	}

	static std::map<int, Vis3DParams> config3D;
	static std::map<int, Vis2DParams> config2D;

	static void init();

};



#endif /* VISUALIZERFACTORY_H */
