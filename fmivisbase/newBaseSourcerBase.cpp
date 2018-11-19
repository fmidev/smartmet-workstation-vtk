#include "newBaseSourcerBase.h"


#include "nbsImpl.h"

#include "nbsMetadata.h"


namespace fmiVis {

	newBaseSourcerBase::newBaseSourcerBase(const std::string &file, nbsMetadata *meta, int param, int res) :
		pimpl(std::make_unique <nbsImpl>(file)), meta(meta), param(param), prevTime(-1), zRes(res)
	{
		zHeight = meta->maxH;
		for (int i = 0; i < 6; i++)
			prevExtent[i] = 0;
	}



	int newBaseSourcerBase::nearestIndex(double time)
	{
		for (auto iter = meta->timeIndex.begin(); iter != meta->timeIndex.end(); iter++) {
			if (iter->first == time) return iter->second;
			auto nextIter = iter;
			nextIter++;
			if (nextIter != meta->timeIndex.end() && nextIter->first > time) return iter->second;
		}
		if (meta->timeIndex.rbegin() != meta->timeIndex.rend()) return meta->timeIndex.rbegin()->second;

		return 0;
	}



	double newBaseSourcerBase::minT()
	{
		return meta->times.front();
	}

	double newBaseSourcerBase::maxT()
	{
		return meta->times.back();
	}


	bool newBaseSourcerBase::LoopParam(int param, int time, std::function<void(int, int, int, float)> f) {
		NFmiFastQueryInfo &dataInfo = pimpl->dataInfo;

		const int subSample = 1;

		int sizeX = meta->sizeX / subSample;
		int sizeY = meta->sizeY / subSample;

		int sizeZ = meta->sizeZ / subSample;

		if (dataInfo.Param(FmiParameterName(param))) {

			dataInfo.TimeIndex(time);



			static std::vector<float> values;

			dataInfo.GetCube(values);


			bool rising = dataInfo.HeightParamIsRising();

			//if (rising) dataInfo.ResetLevel();
			//else dataInfo.LastLevel();

			int ix = 0, iz;

			//do {
			//ix=0;
			//for (dataInfo.ResetLocation(); dataInfo.NextLocation(); ) {
			//do {

			do {
				iz = 0;
				do {

					int x = (ix / subSample) % sizeX;
					int y = (ix / subSample / sizeX) % sizeY;
					int z = iz / subSample;

					float value;

					if (rising) z = sizeZ - 1 - z;

					value = values[z + x * sizeZ + y * sizeZ*sizeX];

					f(x, y, z, value);



					iz += !rising ? subSample : -subSample;
				} while (iz / subSample < sizeZ);

				ix += subSample;
				if (ix%sizeX < subSample)
					ix -= ix % sizeX;
			} while (ix / subSample < sizeX*sizeY);

		}
		else {
			cout << "Failed to find param " << param << std::endl;
			return false;
		}
		return true;
	}

}