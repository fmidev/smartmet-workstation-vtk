#include "VisualizerFactory.h"
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>


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
		20
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
		blueToRedColor(250,420),
		{ 250,420 },
		20
		}));
}
