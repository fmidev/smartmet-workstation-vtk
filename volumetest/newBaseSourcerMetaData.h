#ifndef NEWBASESOURCERMETADATA_H
#define NEWBASESOURCERMETADATA_H

#include <NFmiPoint.h>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>


#include "boost\date_time\posix_time\ptime.hpp"

using namespace boost::posix_time;

//yleisiä tietoja newbase-datasta
struct metaData {
	NFmiPoint p1;
	NFmiPoint p2;
	float minH;
	float maxH;

	long timeSteps;
	float maxHAvailable;
	bool hasHeight;
	int sizeX, sizeY, sizeZ;

	std::vector<double> times;
	std::map<double, int> timeIndex;
	long minT;
	long maxT;

	inline void init(const std::string &file) {
		auto data = NFmiQueryData(file);
		auto dataInfo = NFmiFastQueryInfo(&data);


		hasHeight = false;

		if (dataInfo.Area()) {
			const NFmiArea *a = dataInfo.Area();
			p1 = a->BottomLeftLatLon();
			p2 = a->TopRightLatLon();

		}

		sizeX = dataInfo.GridXNumber();
		sizeY = dataInfo.GridYNumber();
		sizeZ = dataInfo.SizeLevels();


		float minHeight = kMaxFloat, maxHeight = kMinFloat;


		dataInfo.ResetTime();
		dataInfo.NextTime();

		if (dataInfo.Param(kFmiGeopHeight)) {

			hasHeight = true;

			bool rising = dataInfo.HeightParamIsRising();

			if (rising) dataInfo.ResetLevel();
			else dataInfo.LastLevel();

			do {

				for (dataInfo.ResetLocation(); dataInfo.NextLocation(); ) {
					float val = dataInfo.FloatValue();

					if (val == kFloatMissing) val = 0;
					if (val > maxHeight) maxHeight = val;
					if (val < minHeight) minHeight = val;

				}

			} while ((rising && dataInfo.NextLevel()) || (!rising && dataInfo.PreviousLevel()));
		}

		minH = minHeight;
		maxHAvailable = maxHeight;



		int n = 0;

		for (dataInfo.ResetTime(); dataInfo.NextTime(); ) {
			times.push_back(dataInfo.Time().EpochTime());
			timeIndex.insert({ dataInfo.Time().EpochTime(),n });
			n++;
		}

		minT = times.front();
		maxT = times.back();

		timeSteps = times.size()-1;



	}
	static std::string getTimeString(double val) {
		ptime utcTime = ptime(from_time_t(val));
		auto timeString = std::stringstream();

		timeString << to_iso_extended_string(utcTime.date()) << " " << utcTime.time_of_day() << " UTC";

		return timeString.str();
	}

	time_t timeStepToEpoch(double step) {
		return minT + step*(double(maxT - minT) / (timeSteps));
	}
};

#endif /*NEWBASESOURCERMETADATA_H*/