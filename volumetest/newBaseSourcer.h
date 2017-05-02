#ifndef NEWBASESOURCER_H
#define NEWBASESOURCER_H

#include <memory>

#include <vtkImageData.h>
#include <vtkImageAlgorithm.h>


class NFmiQueryData;
class NFmiFastQueryInfo;
struct nbsMetadata;

//lukee sqd-tiedoston ja muuntaa sen vtkImageDataksi
class newBaseSourcer : public vtkImageAlgorithm {


	nbsMetadata *meta;



	vtkImageData* im;
	float* heights;

	int param;

	int prevTime;

	int zRes;
	float zHeight;


	//muunnos epoch-ajasta newbasen aikaindekseiksi ja takaisin

	void resetImage();

	void freeRes() {
		if (im)
			im->Delete();
		if (heights)
			delete[] heights;
	}


public:
	//etsii aikaindeksin annetulle epoch-ajalle
	int nearestIndex(double time);

	newBaseSourcer(const std::string &file, nbsMetadata *meta, int param,int res=70);

	void Delete() override {
		freeRes();
	}


	double minT();
	double maxT();


	//kertoo VTK:lle mitä dataa on saatavilla
	int newBaseSourcer::RequestInformation(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);
	
	//hakee VTK:lle dataa
	int newBaseSourcer::RequestData(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);

private:
	newBaseSourcer::~newBaseSourcer();
	newBaseSourcer(const newBaseSourcer &copy) = delete;
	void operator=(const newBaseSourcer &assign) = delete;

	struct nb;
	std::unique_ptr<nb> pimpl;
};

#endif /*NEWBASESOURCER_H*/