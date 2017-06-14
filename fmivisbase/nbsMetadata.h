#ifndef NEWBASESOURCERMETADATA_H
#define NEWBASESOURCERMETADATA_H


#include <map>
#include <vector>

#include <NFmiPoint.h>


//yleisiä tietoja newbase-datasta
struct nbsMetadata {
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

	void init(const std::string &file);
	static std::string getTimeString(double val);

	time_t timeStepToEpoch(double step) {
		return minT + step*(double(maxT - minT) / (timeSteps));
	}
};

#endif /*NEWBASESOURCERMETADATA_H*/