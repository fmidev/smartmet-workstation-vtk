#ifndef nbsSurfaceImpl_h__
#define nbsSurfaceImpl_h__


#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>

#include "nbsSurface.h"


struct nbsSurface::nbsImpl {

	NFmiQueryData data;
	NFmiFastQueryInfo dataInfo;
	nbsImpl(const std::string &file) : data(file), dataInfo(&data) {}
};

#endif // nbsSurfaceImpl_h__
