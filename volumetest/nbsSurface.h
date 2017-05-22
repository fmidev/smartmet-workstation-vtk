#ifndef nbsSurface_h
#define nbsSurface_h

#include "vtkPolyDataAlgorithm.h"

#include "nbsImpl.h"

class vtkInformation;
class vtkInformationVector;
class vtkDelaunay2D;
class vtkPolyData;
struct nbsMetadata;

class nbsSurface : public vtkPolyDataAlgorithm {

	vtkDelaunay2D *delaunay;
	vtkPolyData *inputPolyData;

	nbsMetadata *meta;

	int param;

	int prevTime;

	float zHeight;

public:
	nbsSurface(const std::string &file, nbsMetadata *meta,int param,int zHeight = 13000);

	int RequestInformation(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);

	int RequestData(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);


	int nearestIndex(double time);
protected:

	struct nbsImpl;
	std::unique_ptr<nbsImpl> pimpl;

	nbsSurface::~nbsSurface();
	nbsSurface(const nbsSurface &copy) = delete;
	void operator=(const nbsSurface &assign) = delete;
};

#endif // nbsWindSurface_h
