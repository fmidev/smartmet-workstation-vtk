#ifndef NEWBASESOURCERMETADATA_H
#define NEWBASESOURCERMETADATA_H


#include <map>
#include <vector>
#include <memory>
#include <array>

#include <NFmiPoint.h>
#include <NFmiArea.h>
#include <NFmiParam.h>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>

typedef std::array<float, 2> paramRange;

//yleisiä tietoja newbase-datasta
struct nbsMetadata {
	NFmiPoint p1;
	NFmiPoint p2;
	float minH;
	float maxH;

	std::shared_ptr<NFmiQueryData> data;
	std::shared_ptr<NFmiFastQueryInfo> dataInfo;

	long timeSteps;
	float maxHAvailable;
	bool hasHeight;
	int sizeX, sizeY, sizeZ;

	std::vector<double> times;
	std::map<double, int> timeIndex;
	long minT;
	long maxT;


	nbsMetadata() :
		data(nullptr),
		dataInfo(nullptr)
	{}

	void init(const std::string &file);
	static std::string getTimeString(double val);

	time_t timeStepToEpoch(double step) {
		return minT + step*(double(maxT - minT) / (timeSteps));
	}

	paramRange getParamRange(int p) const;
};

#endif /*NEWBASESOURCERMETADATA_H*/