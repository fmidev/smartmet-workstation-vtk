#pragma once

#include "NFmiMetTime.h"
#include <boost/shared_ptr.hpp>

class NFmiFastQueryInfo;
class NFmiLevel;

namespace NFmiFastInfoUtils
{
bool IsInfoShipTypeData(NFmiFastQueryInfo &theInfo);
void SetSoundingDataLevel(const NFmiLevel &theWantedSoundingPressureLevel, NFmiFastQueryInfo &info);
std::string GetTotalDataFilePath(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo);
bool IsYearLongData(boost::shared_ptr<NFmiFastQueryInfo> &info);
bool IsModelClimatologyData(boost::shared_ptr<NFmiFastQueryInfo> &info);
NFmiMetTime GetUsedTimeIfModelClimatologyData(boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                                              const NFmiMetTime &theTime);
bool IsMovingSoundingData(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo);
bool FindTimeIndicesForGivenTimeRange(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                                      const NFmiMetTime &theStartTime,
                                      long minuteRange,
                                      unsigned long &timeIndex1,
                                      unsigned long &timeIndex2);
bool FindMovingSoundingDataTime(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                                const NFmiMetTime &theTime,
                                NFmiLocation &theLocation);
}
