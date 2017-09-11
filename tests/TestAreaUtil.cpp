#include <gtest/gtest.h>


#include <boost/shared_ptr.hpp>

#include <NFmiAreaFactory.h>
#include <NFmiArea.h>

#include "areaUtil.h"



TEST(areaUtilTests, IsDataInMapFindsAreaInside) {
	auto scand = AreaUtil::GetScandicArea();
	boost::shared_ptr<NFmiArea> zoomedDataArea = NFmiAreaFactory::Create("stereographic, 20, 90, 60:7, 53, 17, 60");

	ASSERT_TRUE(AreaUtil::IsDataInMap(zoomedDataArea.get(), scand.get()));

}

TEST(areaUtilTests, IsDataInMapTellsDataIsNotInArea) {
	boost::shared_ptr<NFmiArea> zoomedArea = NFmiAreaFactory::Create("stereographic, 20, 90, 60:7, 53, 17, 60");
	boost::shared_ptr<NFmiArea> offDataArea = NFmiAreaFactory::Create("stereographic, -40, 10, 60:7, 23, 5, 60");

	ASSERT_FALSE(AreaUtil::IsDataInMap(offDataArea.get(), zoomedArea.get()));

}

TEST(areaUtilTests, ScandicExtentIdentity) {
	NFmiRect r{ 0,1,1,0 };

	auto ext = AreaUtil::RectIntoExtentScandic(
		AreaUtil::areaToWorldScandic(	r	)	);
	//TODO move to fixture
	ASSERT_EQ(0,ext[0]);
	ASSERT_EQ(89,ext[1]);
	ASSERT_EQ(0,ext[2]);
	ASSERT_EQ(99,ext[3]);
}