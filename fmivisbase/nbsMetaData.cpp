#include "nbsMetadata.h"


#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>


#include "boost\date_time\posix_time\ptime.hpp"



using namespace boost::posix_time;

void nbsMetadata::init(const std::string &file)
{
	data.reset(new NFmiQueryData(file) );
	dataInfo.reset( new NFmiFastQueryInfo(data.get()) );


	hasHeight = false;

	if (dataInfo->Area()) {
		p1 = dataInfo->Area()->BottomLeftLatLon();
		p2 = dataInfo->Area()->TopRightLatLon();

	}

	sizeX = dataInfo->GridXNumber();
	sizeY = dataInfo->GridYNumber();
	sizeZ = dataInfo->SizeLevels();


	float minHeight = kMaxFloat, maxHeight = kMinFloat;


	dataInfo->ResetTime();
	dataInfo->NextTime();

	if (dataInfo->Param(kFmiGeopHeight)) {

		hasHeight = true;

		bool rising = dataInfo->HeightParamIsRising();

		if (rising) dataInfo->ResetLevel();
		else dataInfo->LastLevel();

		do {

			for (dataInfo->ResetLocation(); dataInfo->NextLocation(); ) {
				float val = dataInfo->FloatValue();

				if (val == kFloatMissing) val = 0;
				if (val > maxHeight) maxHeight = val;
				if (val < minHeight) minHeight = val;

			}

		} while ((rising && dataInfo->NextLevel()) || (!rising && dataInfo->PreviousLevel()));
	}

	minH = minHeight;
	maxHAvailable = maxHeight;



	int n = 0;

	for (dataInfo->ResetTime(); dataInfo->NextTime(); ) {
		times.push_back(dataInfo->Time().EpochTime());
		timeIndex.insert({ dataInfo->Time().EpochTime(),n });
		n++;
	}

	minT = times.front();
	maxT = times.back();

	timeSteps = times.size() - 1;
}

std::string nbsMetadata::getTimeString(double val)
{
	ptime utcTime = ptime(from_time_t(val));
	auto timeString = std::stringstream();

	timeString << to_iso_extended_string(utcTime.date()) << " " << utcTime.time_of_day() << " UTC";

	return timeString.str();
}
