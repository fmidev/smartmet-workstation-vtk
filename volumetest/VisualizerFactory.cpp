#include "VisualizerFactory.h"

#include <memory>
#include <map>
#include <utility>

#include <vtkSmartPointer.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>


#include <NFmiParameterName.h>

#include "ParamVisualizerBase.h"
#include "ParamVisualizer2D.h"
#include "ParamVisualizer3D.h"


vtkSmartPointer<vtkColorTransferFunction> VisualizerFactory::blueToRedColor(double min, double max)
{
	auto f = vtkSmartPointer<vtkColorTransferFunction>::New();
	f->AddRGBPoint(min, 0.0, 0.0, 1.0);
	f->AddRGBPoint((min + max) / 2, 0.6, 0, 0.6);
	f->AddRGBPoint(max, 1.0, 0, 0.0);
	return f;
}

vtkSmartPointer<vtkColorTransferFunction> VisualizerFactory::blueToRedColor(double* r) {
	return blueToRedColor(r[0], r[1]);
}

vtkSmartPointer<vtkColorTransferFunction> VisualizerFactory::greenToRedColor(double min, double max)
{
	auto f = vtkSmartPointer<vtkColorTransferFunction>::New();
	f->AddRGBPoint(min, 0.0, 0.5, 0.0);
	f->AddRGBPoint((min + max) / 3, 0, 1, 0);
	f->AddRGBPoint(2 * (min + max) / 3, 0.7, 0.7, 0);
	f->AddRGBPoint(max, 1.0, 0, 0.0);
	return f;
}

vtkSmartPointer<vtkColorTransferFunction> VisualizerFactory::greenToRedColor(double* r) {
	return greenToRedColor(r[0], r[1]);
}
vtkSmartPointer<vtkPiecewiseFunction> VisualizerFactory::opacityFunction(double min, double max, double threshold, double maxAlpha/*=0.2*/, double minAlpha/*=0.0*/)
{
	auto f = vtkSmartPointer<vtkPiecewiseFunction>::New();
	f->AddPoint(min, 0.0f);
	f->AddPoint(threshold, minAlpha);
	f->AddPoint(max, maxAlpha);
	return f;
}

std::unique_ptr<ParamVisualizerBase> VisualizerFactory::make3DVisualizer(const std::string &file, nbsMetadata &meta, int param, vtkSmartPointer<vtkColorTransferFunction> color, vtkSmartPointer<vtkPiecewiseFunction> opacity, double contourColor[3], double contourThreshold, double contourOpacity /*= 1.0f*/)
{
	return std::make_unique<ParamVisualizer3D>(file, meta, param, color, opacity, contourThreshold, contourColor, contourOpacity);
}

std::unique_ptr<ParamVisualizerBase> VisualizerFactory::make3DVisualizer(const std::string &file, nbsMetadata &meta, Vis3DParams &p)
{
	return make3DVisualizer(file, meta, p.param, p.color, p.opacity, p.contourColor, p.contourThreshold, p.contourOpacity);
}

std::unique_ptr<ParamVisualizerBase> VisualizerFactory::make3DVisualizer(const std::string &file, nbsMetadata &meta, int param)
{
	if (config3D.find(param) != config3D.end())
		return make3DVisualizer(file, meta, config3D.find(param)->second);
	return nullptr;
}

std::unique_ptr<ParamVisualizerBase> VisualizerFactory::make2DVisualizer(const std::string &file, nbsMetadata &meta, vtkAlgorithmOutput* probingData, int param, vtkSmartPointer<vtkColorTransferFunction> contourColors, std::shared_ptr<ContourLabeler> labeler, double range[2], int numContours /*= 10*/)
{
	return std::make_unique<ParamVisualizer2D>(file, meta, param, probingData, contourColors, labeler, range, numContours);
}

std::unique_ptr<ParamVisualizerBase> VisualizerFactory::make2DVisualizer(const std::string &file, nbsMetadata &meta, vtkAlgorithmOutput* probingData, std::shared_ptr<ContourLabeler> labeler, Vis2DParams &p)
{
	return make2DVisualizer(file, meta, probingData, p.param, p.contourColors, labeler, p.range, p.numContours);
}

std::unique_ptr<ParamVisualizerBase> VisualizerFactory::make2DVisualizer(const std::string &file, nbsMetadata &meta, vtkAlgorithmOutput* probingData, std::shared_ptr<ContourLabeler> labeler, int param)
{
	if (config2D.find(param) != config2D.end())
		return make2DVisualizer(file, meta, probingData, labeler, config2D.find(param)->second);
	else
		return nullptr;
}

std::map<int, Vis3DParams> VisualizerFactory::config3D;
std::map<int, Vis2DParams> VisualizerFactory::config2D;


void VisualizerFactory::init() {

	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiWindSpeedMS, { kFmiWindSpeedMS,
		greenToRedColor(0, 80),
		opacityFunction(0, 80, 20),
		{ 1,0,1 },
		35,
		1
		}));


	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiTotalCloudCover, { kFmiTotalCloudCover,
		greenToRedColor(0, 100),
		opacityFunction(0, 100, 20),
		{ 0.6,0.6,0.6 },
		50,
		0.8
		}));

	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiCloudWater, { kFmiCloudWater,
		greenToRedColor(0, 0.5),
		opacityFunction(0, 0.5, 0.2),
		{ 1,0,0.5 },
		0.1,
		0.7
		}));

	config3D.insert(std::pair<int, Vis3DParams>(
		277, { 277, //cloud-ice
		greenToRedColor(0, 0.3),
		opacityFunction(0, 0.3, 0.01),
		{ 0.4,0,1 },
		0.08,
		0.9
		}));

	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiIcing, { kFmiIcing,
		blueToRedColor(0, 9),
		opacityFunction(0, 9, 2),
		{ 0.2,0,1 },
		4,
		1.0
		}));


	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiClearAirTurbulence, { kFmiClearAirTurbulence,
		greenToRedColor(0, 150),
		opacityFunction(0, 150, 30),
		{ 1,0,0 },
		40,
		0.5
		}));
	config3D.insert(std::pair<int, Vis3DParams>(
		kFmiClearAirTurbulence2, { kFmiClearAirTurbulence2,
		greenToRedColor(0, 150),
		opacityFunction(0, 150, 30),
		{ 1,0,0 },
		40,
		0.5
		}));


	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiTemperature, { kFmiTemperature ,
		blueToRedColor(-80,30),
		{ -80,30 },
		15
		}));
	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiHumidity, { kFmiHumidity ,
		greenToRedColor(0,100),
		{ 0,100 },
		5
		}));
	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiWindSpeedMS, { kFmiWindSpeedMS ,
		blueToRedColor(0,90),
		{ 0,90 },
		10
		}));
	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiVerticalVelocityMMS, { kFmiVerticalVelocityMMS ,
		blueToRedColor(-200,200),
		{ -200,200 },
		8
		}));
	config2D.insert(std::pair<int, Vis2DParams>(
		kFmiPotentialTemperature, { kFmiPotentialTemperature ,
		blueToRedColor(200,1500),
		{ 200,1500 },
		8
		}));
}
