#ifndef NBSSURFACEWIND_H
#define NBSSURFACEWIND_H

#include "nbsSurface.h"


class vtkInformation;
class vtkInformationVector;

struct nbsMetadata;

class nbsSurfaceWind : public nbsSurface {

public:
	nbsSurfaceWind(const std::string &file, nbsMetadata *meta, int subSample = 1);

	virtual int RequestData(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);
protected:

	int subSample;

	nbsSurfaceWind::~nbsSurfaceWind();
	nbsSurfaceWind(const nbsSurfaceWind &copy) = delete;
	void operator=(const nbsSurfaceWind &assign) = delete;
};

#endif // NBSSURFACEWIND_H
