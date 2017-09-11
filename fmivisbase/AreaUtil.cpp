#include "AreaUtil.h"


AreaUtil::extent AreaUtil::FindExtentScandic(vtkViewport* vp, const NFmiArea *dataArea)
{
	auto scandArea = GetScandicArea();

	auto worldView = GetViewportWolrdArea(vp);
	auto worldScand = ScandicWorldSpace();

	auto worldClip = worldView.Intersection(worldScand);

	if (!worldClip.IsEmpty()) {

		if (IsDataInMap(dataArea, scandArea.get()))
		{
			auto zoomedAreaRect = dataArea->XYArea(scandArea.get());
			AdjustZoomedAreaRect(zoomedAreaRect);

			auto zoomedRect = areaToWorldScandic(zoomedAreaRect);

			auto zoomClip = zoomedRect.Intersection(worldClip);

			if (!zoomClip.IsEmpty())
				return RectIntoExtentScandic(zoomClip);
		}
	}
	return extent{ 0,0,0,0 };
}

NFmiRect AreaUtil::ScandicWorldSpace()
{
	return NFmiRect(0, 2 * 99, 2 * 89, 0);  //TODO spacing & nbs size
}

NFmiRect AreaUtil::areaToWorldScandic(const NFmiRect &a)
{
	auto sw = ScandicWorldSpace();
	NFmiRect ret;
	ret.Place({ sw.Place().X()*a.Place().X(),
					sw.Place().Y()*a.Place().Y() });
	ret.Size({ sw.Width()*a.Width(),
				sw.Height()*a.Height() });
	return ret;
}

AreaUtil::extent AreaUtil::RectIntoExtentScandic(const NFmiRect &a)
{
	return { (int)floor(a.Left() / 2.0), (int)ceil(a.Right() / 2.0), (int)floor(a.Top() / 2.0),(int)ceil(a.Bottom() / 2.0) };
}

boost::shared_ptr<NFmiArea> AreaUtil::GetScandicArea()
{
	static auto area = boost::shared_ptr<NFmiArea>(nullptr);
	if (!area)
		area = NFmiAreaFactory::Create("stereographic, 20, 90, 60:6, 51.3, 49, 70.2");
	return area;
}


bool AreaUtil::IsDataInMap(const NFmiArea * dataArea, const NFmiArea * viewArea)
{
	if (dataArea && viewArea)
	{
		// Tutkitaan ristiin ovatko alueet toistensa sisällä.
		if (dataArea->IsInside(*viewArea))
			return true;
		if (viewArea->IsInside(*dataArea))
			return true;
		// Alue tarkastelut voivat mennä väärin (?), tutkitaan vielä varmuuden vuoksi keskipisteitä.
		if (dataArea->IsInside(viewArea->ToLatLon(viewArea->XYArea().Center())))
			return true;
		if (viewArea->IsInside(dataArea->ToLatLon(dataArea->XYArea().Center())))
			return true;
	}
	return false;
}



NFmiRect AreaUtil::GetViewportWolrdArea(vtkViewport* vp)
{
	std::array<double, 4> buf = { 0,0,0,0 };

	vp->SetViewPoint(-1, -1, 0);
	vp->ViewToWorld();
	vp->GetWorldPoint(&buf[0]);

	auto p0 = HomogenousToCartesian(buf);

	vp->SetViewPoint(1, 1, 0);
	vp->ViewToWorld();
	vp->GetWorldPoint(&buf[0]);

	auto p1 = HomogenousToCartesian(buf);

	return NFmiRect(p0.first, p1.second, p1.first, p0.second);
}
