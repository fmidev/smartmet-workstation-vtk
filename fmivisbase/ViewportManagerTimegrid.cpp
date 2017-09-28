#include "ViewportManagerTimegrid.h"

#include "VisualizerManager.h"

void fmiVis::ViewportManagerTimegrid::UpdateTimeStep(double t)
{
	if (viewports.size() != sizeX*sizeY) throw std::runtime_error("Not enough viewports added yet");

	for (size_t x = 0; x < sizeX; x++)
		for (size_t y = 0; y < sizeY; y++)
			viewports[x + y*sizeX].vm->UpdateTimeStep(t + timeStep*x);
}

void fmiVis::ViewportManagerTimegrid::ViewportNumKey(ViewportID vpid, int vid, bool ctrl)
{
	int row = vpid / sizeX;

	for (size_t x = 0; x < sizeX; x++)
		ViewportManager::ViewportNumKey(x + row*sizeX, vid, ctrl);
}
