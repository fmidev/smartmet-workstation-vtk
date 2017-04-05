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

	std::vector<time_t> times;
	std::map<time_t, int> timeIndex;

	vtkImageData* im;
	float* heights;

	int param;

	int prevTime;

	int zRes;
	float zHeight;


	//muunnos epoch-ajasta newbasen aikaindekseiksi ja takaisin
	inline double epochToDouble(time_t t) {
		return (double(t - minT()) / (maxT() - minT())  * times.size());
	}
	inline time_t doubleToEpoch(double t) {
		return std::lround(t*((maxT() - minT()) / times.size())) + minT();
	}
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

public:
	//etsii aikaindeksin annetulle epoch-ajalle
	inline int nearestIndex(long time) {
		for (auto iter = timeIndex.begin(); iter != timeIndex.end(); iter++) {
			if (iter->first == time) return iter->second;
			auto nextIter = iter;
			nextIter++;
			if (nextIter != timeIndex.end() && nextIter->first > time) return iter->second;
		}
		if (timeIndex.rbegin() != timeIndex.rend()) return timeIndex.rbegin()->second;

		return 0;
	}

	newBaseSourcer(const std::string &file, metaData *meta, int param,int res=70,float height = 13000) :
		data(file), dataInfo(&data), meta(meta), im(nullptr), heights(nullptr), param(param), times(), timeIndex(), prevTime(-1),zRes(res),zHeight(height) {
		SetNumberOfInputPorts(0);
	}
	~newBaseSourcer() {
		if (im)
			im->Delete();
		if (heights)
			delete[] heights;
	}

	inline time_t minT() { return times.front(); }
	inline time_t maxT() { return times.back(); }
	inline double minDT() { return epochToDouble(minT()); }
	inline double maxDT() { return epochToDouble(maxT()); }

	//t‰ytt‰‰ metatiedot
	void initMeta();

	//kertoo VTK:lle mit‰ dataa on saatavilla
	int newBaseSourcer::RequestInformation(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);
	
	//hakee VTK:lle dataa
	int newBaseSourcer::RequestData(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);

};

#endif /*NEWBASESOURCER_H*/