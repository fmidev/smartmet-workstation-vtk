#ifndef newBaseSourcerBase_h__
#define newBaseSourcerBase_h__

#include <functional>
#include <memory>

namespace fmiVis {

	struct nbsMetadata;

	//contains some common functionality of newbase sources, seeking and parameter looping
	class newBaseSourcerBase {

	protected:

		nbsMetadata *meta;


		int param;

		int prevTime;

		int prevExtent[6];

		int zRes;
		float zHeight;


		bool LoopParam(int param, int time, std::function<void(int, int, int, float)> f);

		int nearestIndex(double time);

		double minT();
		double maxT();

		struct nbsImpl;
		std::unique_ptr<nbsImpl> pimpl;

		newBaseSourcerBase(const std::string &file, nbsMetadata *meta, int param, int res);
	public:
		nbsMetadata& getMeta() {
			return *meta;
		}
	};

}

#endif // newBaseSourcerBase_h__
