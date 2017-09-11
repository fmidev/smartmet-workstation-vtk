#ifndef areaUtil_h__
#define areaUtil_h__

#include <vector>
#include <array>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <NFmiAreaFactory.h>
#include <NFmiArea.h>
#include <vtkCamera.h>
#include <vtkViewport.h>


namespace AreaUtil {
	using namespace AreaUtil;
	using std::min;
	using std::max;

	typedef std::pair<double, double> point;

	typedef std::array<int, 4> extent;



	extent FindExtentScandic(vtkViewport* vp, const NFmiArea *dataArea);

	bool IsDataInMap(const NFmiArea * dataArea, const NFmiArea * viewArea);

	NFmiRect areaToWorldScandic(const NFmiRect &a);

	extent RectIntoExtentScandic(const NFmiRect &a);

	NFmiRect ScandicWorldSpace();


	NFmiRect GetViewportWolrdArea(vtkViewport* vp);


	inline point HomogenousToCartesian(std::array<double, 4> &in) {
		if (abs(in[3]) < 0.0001) throw std::runtime_error("Homogenous point at infinity");

		return point{ in[0] / in[3], in[1] / in[3] };
	}

	inline void AdjustZoomedAreaRect(NFmiRect &theZoomedAreaRect) {
		// HUOM!!! viilaan zoom laatikon hienoista epätarkkuutta
		static const double errorLimit = 0.000001;
		if (fabs(theZoomedAreaRect.Left()) < errorLimit)
			theZoomedAreaRect.Left(0);
		if (fabs(theZoomedAreaRect.Top()) < errorLimit)
			theZoomedAreaRect.Top(0);
		if (fabs(1. - theZoomedAreaRect.Right()) < errorLimit)
			theZoomedAreaRect.Right(1);
		if (fabs(1. - theZoomedAreaRect.Bottom()) < errorLimit)
			theZoomedAreaRect.Bottom(1);
	}


	boost::shared_ptr<NFmiArea> GetScandicArea();


}



#endif // areaUtil_h__
