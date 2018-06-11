#ifndef nbsSurface_h
#define nbsSurface_h

#include "vtkPolyDataAlgorithm.h"
#include <vtkFloatArray.h>
#include <vtksmartpointer.h>
#include <vtkPoints.h>

#include "nbsImpl.h"
#include "newBaseSourcerBase.h"

class vtkInformation;
class vtkInformationVector;
class vtkDelaunay2D;
class vtkPolyData;
struct nbsMetadata;

class nbsSurface : public vtkPolyDataAlgorithm, public newBaseSourcerBase {
protected:
	vtkPolyData *inputPolyData;

	bool flat;
	bool pad;

	bool loadPoints();

	vtkSmartPointer<vtkFloatArray> textureCoordinates;
	vtkSmartPointer<vtkFloatArray> normals;


	vtkSmartPointer<vtkFloatArray> scalars;

	vtkSmartPointer<vtkPoints> points;

public:
	nbsSurface(const std::string &file, nbsMetadata *meta,int param,int zHeight = 13000, bool flat = false, bool pad = false);

	int RequestInformation(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);

	int RequestData(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);

protected:

	int spacing;
	nbsSurface::~nbsSurface();
	nbsSurface(const nbsSurface &copy) = delete;
	void operator=(const nbsSurface &assign) = delete;
};

#endif // nbsWindSurface_h
