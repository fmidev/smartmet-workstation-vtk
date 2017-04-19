#ifndef NEWBASESOURCER_H
#define NEWBASESOURCER_H

#include "newBaseSourcerMetaData.h"

#include <vtkImageAlgorithm.h>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>

#include <NFmiArea.h>

#include <vtkImageData.h>

//lukee sqd-tiedoston ja muuntaa sen vtkImageDataksi
class newBaseSourcer : public vtkImageAlgorithm {

	NFmiQueryData data;
	NFmiFastQueryInfo dataInfo;
	metaData *meta;



	vtkImageData* im;
	float* heights;

	int param;

	int prevTime;

	int zRes;
	float zHeight;


	//muunnos epoch-ajasta newbasen aikaindekseiksi ja takaisin

	inline void resetImage() {
		assert(im);
		int sizeX=meta->sizeX, sizeY=meta->sizeY;
		float* p = static_cast<float*>(im->GetScalarPointer());
		for (long iz = 0; iz < zRes; ++iz) {
			for (long iy = 0; iy < sizeX; ++iy) {
				for (long ix = 0; ix < sizeX; ++ix) {
					p[ix + iy*sizeX + iz*sizeX*sizeY] = kFloatMissing;
				}
			}
		}
	}

	void freeRes() {
		if (im)
			im->Delete();
		if (heights)
			delete[] heights;
	}


public:
	//etsii aikaindeksin annetulle epoch-ajalle
	inline int nearestIndex(double time) {
		for (auto iter = meta->timeIndex.begin(); iter != meta->timeIndex.end(); iter++) {
			if (iter->first == time) return iter->second;
			auto nextIter = iter;
			nextIter++;
			if (nextIter != meta->timeIndex.end() && nextIter->first > time) return iter->second;
		}
		if (meta->timeIndex.rbegin() != meta->timeIndex.rend()) return meta->timeIndex.rbegin()->second;

		return 0;
	}

	newBaseSourcer(const std::string &file, metaData *meta, int param,int res=70) :
		data(file), dataInfo(&data), meta(meta),
		im(nullptr), heights(nullptr), 
		param(param), prevTime(-1),zRes(res)
	{
		SetNumberOfInputPorts(0);
		zHeight = meta->maxH;
	}

	void Delete() override {
		freeRes();
	}


	inline double minT() { return meta->times.front(); }
	inline double maxT() { return meta->times.back(); }


	//kertoo VTK:lle mitä dataa on saatavilla
	int newBaseSourcer::RequestInformation(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);
	
	//hakee VTK:lle dataa
	int newBaseSourcer::RequestData(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);

private:
	~newBaseSourcer() override {
	}
	newBaseSourcer(const newBaseSourcer &copy) = delete;
	void operator=(const newBaseSourcer &assign) = delete;
};

#endif /*NEWBASESOURCER_H*/