#ifndef nbsImpl_h
#define nbsImpl_h

#include "newBaseSourcer.h"

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>

namespace fmiVis {

	//pimpl
	struct newBaseSourcerBase::nbsImpl {

		NFmiQueryData data;
		NFmiFastQueryInfo dataInfo;
		nbsImpl(const std::string &file) : data(file), dataInfo(&data) {}
	};

}

#endif // nbsImpl_h
