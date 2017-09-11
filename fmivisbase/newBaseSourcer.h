#ifndef NEWBASESOURCER_H
#define NEWBASESOURCER_H

#include <memory>
#include <functional>
#include <vector>

#include <vtkImageData.h>
#include <vtkImageAlgorithm.h>
#include "newBaseSourcerBase.h"


class NFmiQueryData;
class NFmiFastQueryInfo;
struct nbsMetadata;

//lukee sqd-tiedoston ja muuntaa sen vtkImageDataksi
class newBaseSourcer : public newBaseSourcerBase, public vtkImageAlgorithm {

protected:
	vtkImageData* im;
//	float* heights;
	std::vector<float> heights;


	int subSample;

	//muunnos epoch-ajasta newbasen aikaindekseiksi ja takaisin

	virtual void ResetImage(bool realloc = false);
	void AllocateHeights();

	int getHeight(int x, int y, int z);

	void ReadHeights(int time);

	inline void freeRes() {
		if (im)
			im->Delete();
// 		if (heights)
// 			delete[] heights;
	}


public:
	//etsii aikaindeksin annetulle epoch-ajalle

	newBaseSourcer(const std::string &file, nbsMetadata *meta, int param,int res=80, int subSample = 1);

	void Delete() override {
		freeRes();
	}


	//kertoo VTK:lle mitä dataa on saatavilla
	int newBaseSourcer::RequestInformation(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);
	
	//hakee VTK:lle dataa
	virtual int newBaseSourcer::RequestData(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);

	void setSubSample(int s) {
		subSample = s;
		ResetImage(true);
		AllocateHeights();
		prevTime = -1;
	}

	inline int getParam() {
		return param;
	}
protected:
	virtual newBaseSourcer::~newBaseSourcer();
	newBaseSourcer(const newBaseSourcer &copy) = delete;
	void operator=(const newBaseSourcer &assign) = delete;


};

#endif /*NEWBASESOURCER_H*/