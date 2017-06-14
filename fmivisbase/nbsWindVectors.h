#ifndef nbsWindVectors_h
#define nbsWindVectors_h

#include "newBaseSourcer.h"


class vtkInformation;
class vtkInformationVector;

struct nbsMetadata;

class nbsWindVectors : public newBaseSourcer {
	void ResetImage(bool realloc = false);

public:
	nbsWindVectors(const std::string &file, nbsMetadata *meta, int res = 80, int subSample = 1);

	virtual int RequestData(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);
protected:

	nbsWindVectors::~nbsWindVectors();
	nbsWindVectors(const nbsWindVectors &copy) = delete;
	void operator=(const nbsWindVectors &assign) = delete;
};

#endif // nbsWindVectors_h
