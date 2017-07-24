
#include <vector>
#include <chrono>
#include <iomanip>
#include <numeric>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>

#include <NFmiDataMatrix.h>
#include "NFmiMetTime.h"

typedef std::chrono::duration<float,std::milli> ms;


void newGet(NFmiFastQueryInfo &info,std::vector<float> &values) {
	

	info.GetLevelToVec(values);
	//	std::cout << "GetLevelToVec failed" << std::endl;

}

void oldGet(NFmiFastQueryInfo &info, NFmiDataMatrix<float> &values) {
	info.Values(values);
}

void naiveGet(NFmiFastQueryInfo &info, std::vector<float> &values) {

	values.resize(info.SizeLocations());

	int i = 0;
	info.ResetLocation();
	while (info.NextLocation()) {
		values[i++] = info.FloatValue();
	}

}

void testOldGet(NFmiFastQueryInfo &info) {

	auto values = std::vector< NFmiDataMatrix<float> >{};
	for (auto i = 0; i < info.SizeParams()*info.SizeLevels()*info.SizeTimes(); ++i)
		values.emplace_back(info.GridXNumber(), info.GridYNumber());


	auto pos = values.begin();

	for (info.ResetParam(); info.NextParam(); )
	{
		for (info.ResetLevel(); info.NextLevel(); )
		{
			for (info.ResetTime(); info.NextTime(); )
			{
				for (info.ResetLocation(); info.NextLocation(); ) 
				{
					*pos = info.FloatValue();
					pos++;
				}

			}

		}
	}

}

void testNewGet(NFmiFastQueryInfo &info) {

	auto values = std::vector< std::vector<float> >{};
	for (auto i = 0; i < info.SizeTimes(); ++i)
		values.emplace_back(info.SizeLocations()*info.SizeLevels(), 0);


	auto t1 = std::chrono::high_resolution_clock::now();

	auto pos = values.begin();

	for (info.ResetParam(); info.NextParam(); )
	{
		for (info.ResetLevel(); info.NextLevel(); )
		{
			for (info.ResetTime(); info.NextTime(); )
			{

				newGet(info, *pos);
				pos++;

			}

		}
	}



	std::cout << "Time:\t" << std::setprecision(8) << std::chrono::duration_cast<ms>((std::chrono::high_resolution_clock::now() - t1)).count() << std::endl;


	double sum = 0;
	for each(const auto &v in values)
				sum = std::accumulate(v.begin(), v.end(), sum);

	std::cout << "total sum: " << sum
		<< " of " << values.size() << " elements. " << std::endl;


}

void testNaiveGet(NFmiFastQueryInfo &info) {

	auto values = std::vector<float>(
		info.SizeParams()*info.SizeLocations()*info.SizeLevels()*info.SizeTimes(),0.0f );


	auto t1 = std::chrono::high_resolution_clock::now();



	int p = 0;
	for (info.ResetParam(); info.NextParam(); )
	{
		int x = 0;
		for (info.ResetLocation(); info.NextLocation(); ) {
			int z = 0;
			for (info.ResetLevel(); info.NextLevel(); )
			{
				int t = 0;
				for (info.ResetTime(); info.NextTime(); )
				{

					values[t + z*info.SizeTimes()
						+ x*info.SizeTimes()*info.SizeLevels()
						+ p*info.SizeTimes()*info.SizeLevels()*info.SizeParams()] = info.FloatValue();

					//pos++;
					++t;
				}
				++z;
			}
			++x;
		}
		++p;
	}


	std::cout << "Time:\t" << std::setprecision(8) << std::chrono::duration_cast<ms>((std::chrono::high_resolution_clock::now() - t1)).count() << std::endl;


	double sum = 0;
	for each(const auto &v in values)
		sum = std::accumulate(values.begin(), values.end(), sum);
	std::cout << "total sum: " << sum
		<< " of " << values.size() << " elements. " << std::endl;

}

void testCube(NFmiFastQueryInfo &info) {
	auto values = std::vector< std::vector<float> >{};
	for (auto i = 0; i < info.SizeTimes()*info.SizeParams(); ++i)
		values.emplace_back(info.SizeLocations()*info.SizeLevels(), 0);

	auto t1 = std::chrono::high_resolution_clock::now();

	auto pos = values.begin();

	for (info.ResetParam(); info.NextParam(); )
	{
		for (info.ResetTime(); info.NextTime(); )
		{

			info.GetCube(*pos);
			pos++;

		}
	}

	std::cout << "Time:\t" << std::setprecision(8) << std::chrono::duration_cast<ms>((std::chrono::high_resolution_clock::now() - t1)).count() << std::endl;

	double sum = 0;
	for each(const auto &v in values)
		sum = std::accumulate(v.begin(), v.end(), sum);
	//		for each(const auto &u in v)
	//			sum = std::accumulate(u.begin(), u.end(), sum);
	//	sum = std::accumulate(values.begin(), values.end(), sum);
	std::cout << "total sum: " << sum
		<< " of " << values.size() << " elements. " << std::endl;
}

void testInterpCube(NFmiFastQueryInfo &info) {



	info.FirstTime();

	auto firstTime = info.ValidTime();

	info.NextTime();

	auto secondTime = info.ValidTime();

	auto interpTime = firstTime;

	interpTime.SetHour(((int)firstTime.GetHour() + (int)secondTime.GetHour()) / 2);

	auto values = std::vector< std::vector<float> >{};
	for (auto i = 0; i < info.SizeTimes()*info.SizeParams(); ++i)
		values.emplace_back(info.SizeLocations()*info.SizeLevels(), 0);

	auto t1 = std::chrono::high_resolution_clock::now();

	auto pos = values.begin();

	info.FirstParam();

	for (info.ResetParam(); info.NextParam(); )
	{
		for (info.ResetTime(); info.NextTime(); )
		{

			info.GetInterpolatedCube(*pos, interpTime);
			pos++;

		}
	}

	std::cout << "Time:\t" << std::setprecision(8) << std::chrono::duration_cast<ms>((std::chrono::high_resolution_clock::now() - t1)).count() << std::endl;

	double sum = 0;
	for each(const auto &v in values)
		sum = std::accumulate(v.begin(), v.end(), sum);


}

int main(size_t argc, char* argv[] )
{


	std::string file = std::string(argc < 2 ?
		"D:/3D-dataa/201703210327_hirlam_skandinavia_mallipinta.sqd" :
		argv[1]
	);

	auto data = NFmiQueryData(file);

	auto info = NFmiFastQueryInfo(&data);

	testInterpCube(info);

	char n;

	std::cin >> n;

    return 0;
}


