#include "NFmiFastInfoUtils.h"
#include "NFmiFastQueryInfo.h"
#include "NFmiFileString.h"
#include "NFmiProducerName.h"

namespace NFmiFastInfoUtils
{
bool IsInfoShipTypeData(NFmiFastQueryInfo &theInfo)
{
  if (theInfo.IsGrid() == false)
  {
    FmiProducerName prodId = static_cast<FmiProducerName>(theInfo.Producer()->GetIdent());
    if (prodId == kFmiSHIP || prodId == kFmiBUOY) return true;
  }
  return false;
}

// synop-dataa ei tarvitsee asettaa mihink‰‰n, mutta
// mm. luotaus plotin yhteydess‰ pit‰‰ etsi‰ oikea korkeus/level
void SetSoundingDataLevel(const NFmiLevel &theWantedSoundingPressureLevel, NFmiFastQueryInfo &info)
{
  if (theWantedSoundingPressureLevel.LevelType() == kFmiSoundingLevel)
  {  // sounding plotin yhteydess‰ pit‰‰ asettaa level haluttuun korkeuteen
    float levelValue = theWantedSoundingPressureLevel.LevelValue();
    FmiParameterName parName = static_cast<FmiParameterName>(info.Param().GetParamIdent());
    unsigned long parIndex = info.ParamIndex();
    bool subParaUsed = info.IsSubParamUsed();
    if (info.Param(kFmiPressure))
    {
      for (info.ResetLevel(); info.NextLevel();)
        if (info.FloatValue() == levelValue) break;
    }
    if (subParaUsed)
      info.Param(parName);  // pakko vet‰‰ t‰m‰ hitaalla tavalla jostain syyst‰
    else
      info.ParamIndex(parIndex);
  }
}

std::string GetTotalDataFilePath(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo)
{
  NFmiFileString totalFilePath(theInfo->DataFilePattern());
  totalFilePath.FileName(theInfo->DataFileName());
  return std::string(totalFilePath);
}

bool IsYearLongData(boost::shared_ptr<NFmiFastQueryInfo> &info)
{
  const auto &timeDescriptor = info->TimeDescriptor();
  auto timeDiffInDays = timeDescriptor.LastTime().DifferenceInDays(timeDescriptor.FirstTime());
  if (timeDiffInDays >= 364 && timeDiffInDays <= 366)
    return true;
  else
    return false;
}

bool IsModelClimatologyData(boost::shared_ptr<NFmiFastQueryInfo> &info)
{
  if (info && info->DataType() == NFmiInfoData::kClimatologyData)
  {
    if (info->IsGrid())
    {
      if (IsYearLongData(info))
      {
        return true;
      }
    }
  }
  return false;
}

NFmiMetTime GetUsedTimeIfModelClimatologyData(boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                                              const NFmiMetTime &theTime)
{
  if (NFmiFastInfoUtils::IsModelClimatologyData(theInfo))
  {
    // For year long climatology data, used time must be fixed to data's own year
    auto usedTime(theTime);
    usedTime.SetYear(theInfo->TimeDescriptor().FirstTime().GetYear());
    return usedTime;
  }
  else
    return theTime;
}

bool IsMovingSoundingData(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo)
{
  if (theInfo && !theInfo->IsGrid())
  {
    if (theInfo->SizeLevels() > 7) return theInfo->HasLatlonInfoInData();
  }
  return false;
}

bool FindTimeIndicesForGivenTimeRange(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                                      const NFmiMetTime &theStartTime,
                                      long minuteRange,
                                      unsigned long &timeIndex1,
                                      unsigned long &timeIndex2)
{
  theInfo->FindNearestTime(theStartTime, kForward);
  timeIndex1 = theInfo->TimeIndex();
  NFmiMetTime endTime(theStartTime);
  endTime.ChangeByMinutes(minuteRange);
  theInfo->FindNearestTime(endTime, kBackward);
  timeIndex2 = theInfo->TimeIndex();

  if(timeIndex1 == gMissingIndex || timeIndex2 == gMissingIndex)
      return false;
  else if (timeIndex1 == timeIndex2)  // pit‰‰ testata erikois tapaus, koska TimeToNearestStep-palauttaa
                                 // aina jotain, jos on dataa
  {
    theInfo->TimeIndex(timeIndex1);
    NFmiMetTime foundTime(theInfo->Time());
    if (foundTime > endTime || foundTime < theStartTime)  // jos lˆydetty aika on alku ja loppu ajan
                                                          // ulkopuolella ei piirret‰ salamaa
      return false;
  }
  return true;
}

bool FindMovingSoundingDataTime(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                                const NFmiMetTime &theTime,
                                NFmiLocation &theLocation)
{
  theInfo->FirstLocation();  // liikkuvissa luotauksissa vain yksi dummy paikka, laitetaan se p‰‰lle
  NFmiMetTime timeStart(theTime);
  timeStart.ChangeByMinutes(-30);
  unsigned long timeIndex1 = 0;
  unsigned long timeIndex2 = 0;
  if (FindTimeIndicesForGivenTimeRange(theInfo, timeStart, 60, timeIndex1, timeIndex2) == false)
    return false;

  float lat = 0;
  float lon = 0;
  theInfo->Param(kFmiLatitude);
  unsigned long latIndex = theInfo->ParamIndex();
  theInfo->Param(kFmiLongitude);
  unsigned long lonIndex = theInfo->ParamIndex();
  double minDistance = 99999999;
  unsigned long minDistTimeInd = static_cast<unsigned long>(-1);
  for (unsigned long i = timeIndex1; i <= timeIndex2; i++)
  {
    theInfo->TimeIndex(i);

    for (theInfo->ResetLevel(); theInfo->NextLevel();)
    {
      theInfo->ParamIndex(latIndex);
      lat = theInfo->FloatValue();
      theInfo->ParamIndex(lonIndex);
      lon = theInfo->FloatValue();

      if (lat != kFloatMissing && lon != kFloatMissing)
      {
        NFmiLocation loc(NFmiPoint(lon, lat));
        double currDist = theLocation.Distance(loc);
        if (currDist < minDistance)
        {
          minDistance = currDist;
          minDistTimeInd = i;
        }
      }
    }
  }
  if (minDistance <
      1000 * 1000)  // jos liikkuva luotaus paikka lˆytyi v‰hint‰in 1000 km s‰teelt‰ hiiren
                    // klikkauspaikasta, otetaan kyseinen luotaus piirtoon
  {
    theInfo->TimeIndex(minDistTimeInd);
    // pit‰‰ lis‰ksi asettaa locationiksi luotauksen alkupiste
    theInfo->FirstLevel();
    theInfo->ParamIndex(latIndex);
    lat = theInfo->FloatValue();
    theInfo->ParamIndex(lonIndex);
    lon = theInfo->FloatValue();
    theLocation.SetLatitude(lat);
    theLocation.SetLongitude(lon);

    return true;
  }

  return false;
}
}
