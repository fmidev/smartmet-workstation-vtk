
#include <vector>
#include <chrono>
#include <iomanip>
#include <numeric>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>

typedef std::chrono::duration<float,std::milli> ms;


void newGet(NFmiFastQueryInfo &info,std::vector<float> &values) {
	

	info.GetLevelToVec(values);
	//	std::cout << "GetLevelToVec failed" << std::endl;

}

void oldGet(NFmiFastQueryInfo &info, std::vector<float> &values) {

	values.resize(info.SizeLocations());

	int i = 0;
	info.ResetLocation();
	while (info.NextLocation()) {
		values[i++] = info.FloatValue();
	}
}

int main(size_t argc, char* argv[] )
{


	std::string file = std::string(argc < 2 ?
		"D:/3D-dataa/201703210327_hirlam_skandinavia_mallipinta.sqd" :
		argv[1]
	);

	auto data = NFmiQueryData(file);
	auto info = NFmiFastQueryInfo(&data);

	double sum = 0;

	auto values = std::vector< std::vector<float> >{};
	for (auto i = 0; i < info.SizeParams()*info.SizeLevels()*info.SizeTimes(); ++i)
		values.emplace_back(0, info.SizeLocations());

	std::cout << std::fixed;


	auto t1 = std::chrono::high_resolution_clock::now();

	auto pos = values.begin();

	for (info.ResetParam(); info.NextParam(); )
	{
		for (info.ResetLevel(); info.NextLevel(); )
		{
			for (info.ResetTime(); info.NextTime(); )
			{

				oldGet(info,*pos);
				pos++;

			}

		}
	}

	std::cout << "Time:\t" << std::setprecision(8) << std::chrono::duration_cast<ms>((std::chrono::high_resolution_clock::now() - t1)).count() << std::endl;


	for each(const auto &v in values)
		sum = std::accumulate(v.begin(), v.end(), sum);

	std::cout << "total sum: " << sum
		<< " of " << values.size() << " elements. " << std::endl;

	int n;
	std::cin >> n;

    return 0;
}


