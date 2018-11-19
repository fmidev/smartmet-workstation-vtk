#ifndef areaUtil_h__
#define areaUtil_h__

#include <vector>
#include <array>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <NFmiAreaFactory.h>
#include <NFmiArea.h>
#include <vtkCamera.h>
#include <vtkViewport.h>

namespace fmiVis {

	struct nbsMetadata;


	//AreaUtil contains functions to handle transformations between screen-, VTK- and newbase space
	namespace AreaUtil {
		using std::min;
		using std::max;

		typedef std::pair<double, double> point;

		typedef std::array<int, 4> extent;


		//transform 2D camera's view to newbase coordinates
		extent FindExtentScandic(vtkViewport* vp, const  nbsMetadata &meta);

		//newbase boilerplate
		bool IsDataInMap(const NFmiArea * dataArea, const NFmiArea * viewArea);

		//these functions are used to transform the newbase coordinates into the internal VTK space

		NFmiRect areaToWorldScandic(const NFmiRect &a, const nbsMetadata &meta);
		NFmiRect areaToWorldScandic(const NFmiRect &a, const NFmiRect sw);

		extent RectIntoExtentScandic(const NFmiRect &a);

		NFmiRect ScandicWorldSpace(int sizeX, int sizeY);
		NFmiRect ScandicWorldSpace(const nbsMetadata &meta);


		NFmiRect GetViewportWolrdArea(vtkViewport* vp);


		inline point HomogenousToCartesian(std::array<double, 4> &in) {
			if (abs(in[3]) < 0.0001) throw std::runtime_error("Homogenous point at infinity");

			return point{ in[0] / in[3], in[1] / in[3] };
		}

		//fuzzying inaccuracy of the zoom, or something
		inline void AdjustZoomedAreaRect(NFmiRect &theZoomedAreaRect) {
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

		//hardcoded area of the map in newbase space
		boost::shared_ptr<NFmiArea> GetScandicArea();


	}


}
#endif // areaUtil_h__
