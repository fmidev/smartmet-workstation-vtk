#ifndef NEWBASESOURCERMETADATA_H
#define NEWBASESOURCERMETADATA_H

#include <NFmiPoint.h>


//yleisiä tietoja newbase-datasta
struct metaData {
	NFmiPoint p1;
	NFmiPoint p2;
	float minH;
	float maxH;
	bool hasHeight;
	int sizeX, sizeY, sizeZ;
};

#endif /*NEWBASESOURCERMETADATA_H*/