#include "newBaseSourcer.h"

#include <future>

#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include "boost/date_time/posix_time/posix_time.hpp"



int newBaseSourcer::RequestInformation(vtkInformation* vtkNotUsed(request),
	vtkInformationVector** vtkNotUsed(inputVector),
	vtkInformationVector* outputVector) {

	//cout << "start time: " << minT() << ", double conversion: " << minDT() << std::endl;
	//cout << "end time: " << maxT()<< ", double conversion: " << maxDT() << std::endl;



	vtkInformation* outInfo = outputVector->GetInformationObject(0);


	double doubleRange[2];
	doubleRange[0] = meta->times.front();
	doubleRange[1] = meta->times.back();


	outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), doubleRange, 2);
	outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &meta->times[0], meta->times.size());


	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 0, meta->sizeX - 1, 0, meta->sizeY - 1, 0, zRes - 1);
	return 1;
}


int newBaseSourcer::RequestData(vtkInformation* vtkNotUsed(request),
	vtkInformationVector** vtkNotUsed(inputVector),
	vtkInformationVector* outputVector) {

	float zScale = 1;

	int sizeX = meta->sizeX;
	int sizeY = meta->sizeY;
	int sizeZ = meta->sizeZ;

	//sizeZ = zRes;

	if (!im) {
		im = vtkImageData::New();
		im->Initialize();
		im->SetDimensions(sizeX, sizeY, zRes);
		im->SetSpacing(2, 2, 2);
		im->AllocateScalars(VTK_FLOAT, 1);

		resetImage();
	}

	unsigned long time = 0;

	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	//pyydetäänkö tiettyä ajanjaksoa
	if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP())) {
		time = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());
		cout << param << ": Request for time " << time << std::endl;
	}

	//pyydetäänkö jotain aluetta (näyttäisi olevan aina koko alue)
	int* reqExtent = nullptr;
	if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT())) {
		reqExtent = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT());
		cout << param << ": Requested extent " << reqExtent[0] << "-" << reqExtent[1] << ", "
			<< reqExtent[2] << "-" << reqExtent[3] << ", " << reqExtent[4] << "-" << reqExtent[5] << std::endl;
	}

	vtkDataObject *ds = outInfo->Get(vtkDataSet::DATA_OBJECT());

	int timeI = nearestIndex(time);

	//onko aika-askel jo muistissa
	if (timeI != prevTime) {

		resetImage();

		dataInfo.ResetTime();

		float minVal = kMaxFloat, maxVal = kMinFloat;



		heights;
		if (heights == nullptr) {
			heights = new float[sizeX*sizeY*sizeZ];
		}

		float maxHeight = -1;

		
		auto t0 = std::chrono::system_clock::now();

		//luetaan datapisteiden korkeudet
		if (dataInfo.Param(kFmiGeopHeight)) {

			dataInfo.TimeIndex(timeI);
			int ix, iz = 0;

			bool rising = dataInfo.HeightParamIsRising();

			if (rising) dataInfo.ResetLevel();
			else dataInfo.LastLevel();

			do {
				float h;
				float totalH = 0;
				ix = 0;
				for (dataInfo.ResetLocation(); dataInfo.NextLocation(); ) {

					int x = ix % sizeX;
					int y = (ix / sizeX) % sizeY;
					int z = iz;

					float val = dataInfo.FloatValue();



					if (val == kFloatMissing) {
						val = 0;
					}

					if (val > maxHeight)
						maxHeight = val;

					h = (val - meta->minH) / zHeight * float(sizeZ);

					heights[x + y *sizeX + z*sizeX*sizeY] = h;
					//totalH += h;
					ix++;
				}
				//totalH /= sizeX*sizeY;
				//cout << "Z: " << iz << ", h: " << totalH<<endl;
				iz++;

			} while ((rising && dataInfo.NextLevel()) || (!rising && dataInfo.PreviousLevel()));

		}

		cout << "MaxHeight for time " << timeI << " is " << maxHeight << std::endl;

		//for (int i = 0; i < sizeZ; ++i)
		//	cout << heights[40+40*sizeX+i*sizeX*sizeY] << ", ";

		int highest = -1;


		//luetaan parametri
		if (dataInfo.Param(FmiParameterName(param))) {

			dataInfo.TimeIndex(timeI);
			int ix, iz = 0;

			bool rising = dataInfo.HeightParamIsRising();

			if (rising) dataInfo.ResetLevel();
			else dataInfo.LastLevel();

			float step = zHeight / zRes;

			do {

				//if (iz == zRes) break;

				ix = 0;
				for (dataInfo.ResetLocation(); dataInfo.NextLocation(); ) {
					int x = ix % sizeX;
					int y = (ix / sizeX) % sizeY;
					int z = iz*zScale;
					float pz = iz*step;
					if (meta->hasHeight) {
						z = heights[x + y*sizeX + z*sizeX*sizeY];
						if (z >= zRes) z = zRes;
					}
					if (reqExtent) {
						if (x<reqExtent[0] || x>reqExtent[1]
							|| y<reqExtent[2] || y>reqExtent[3]
							|| z<reqExtent[4] || z>reqExtent[5]) {
							ix++;
							continue;
						}

					}

					float val = dataInfo.FloatValue()*zScale;
					if (val != kFloatMissing) {
						//val = 0.0f; //voi myös piirtää laittamalla negatiiviseksi tjms
						if (val > maxVal) maxVal = val;
						if (val < minVal) minVal = val;
					}
					highest = z;

					if (z < zRes) {

						float* pixel = static_cast<float*>(im->GetScalarPointer(x, y, z));


						pixel[0] = val;

						if (pixel[0] > maxVal) maxVal = pixel[0];
					}
					ix++;
				}
				iz++;

			} while ((rising && dataInfo.NextLevel()) || (!rising && dataInfo.PreviousLevel()));
		}
		else {
			cout << "Failed to find param!" << std::endl;
			return 0;
		}
		float magnitude = maxVal - minVal;

		cout << param << ": max: " << maxVal << " min: " << minVal << ", mag: " << magnitude << std::endl;
		cout << "highest value at " << highest << std::endl;

		auto t1 = std::chrono::system_clock::now();

		//for (int i = 0; i<zRes; ++i)
		//	cout << "z: " << i << " : " << static_cast<float*>(im->GetScalarPointer(40, 40, i))[0] << endl;

		//cout << "interpolating..." << endl;

		unsigned int usedThreadCount = boost::thread::hardware_concurrency();
		auto threads = std::list<std::shared_future<void>>();

		for (int i = 0; i < 9; ++i) {
			int blockX = i % 3;
			int blockY = i / 3;

			int startX = blockX*sizeX / 3;
			int startY = blockY*sizeY / 3;

			int endX = startX + sizeX / 3;
			int endY = startY + sizeY / 3;

			if (threads.size() >= usedThreadCount) {
				auto iter = threads.begin();
				while (iter != threads.end())
				{

					if (iter->wait_for(std::chrono::milliseconds(4)) == std::future_status::ready) {
						iter = threads.erase(iter);
					}
					else ++iter;
				}
			}
			threads.push_back(std::async(std::launch::async, [=]
			{
				for (int ix = startX; ix < endX; ++ix) {
					for (int iy = startY; iy < endY; ++iy) {
						float prevVal = 0, nextVal;
						for (int iz = 0; iz < zRes; ++iz)
						{
							float* val = static_cast<float*>(im->GetScalarPointer(ix, iy, iz));
							if (*val == kFloatMissing)
							{
								if (iz == 0 || iz == zRes - 1) *val = prevVal;
								for (int findNext = iz + 1; findNext < zRes; ++findNext) {
									float val2 = static_cast<float*>(im->GetScalarPointer(ix, iy, findNext))[0];
									if (val2 != kFloatMissing || findNext == zRes - 1) {
										if (val2 == kFloatMissing && findNext == zRes - 1) nextVal = prevVal;
										else nextVal = val2;
										for (int interpolateBack = findNext; interpolateBack >= iz; --interpolateBack) {
											float alpha = float(findNext - interpolateBack) / float(findNext - iz + 1);
											float* pixel = static_cast<float*>(im->GetScalarPointer(ix, iy, interpolateBack));
											pixel[0] = prevVal*alpha + nextVal*(1.0f - alpha);
										}
										iz = findNext;
										prevVal = val2;
										break;
									}
								}
							}
							else prevVal = *val;
						}
					}
				}
			}).share() );
		}
		//for(int i=0;i<zRes;++i)
		//	cout << "z: "<<i<<" : "<<static_cast<float*>(im->GetScalarPointer(40, 40, i))[0]<<endl;


		auto t2 = std::chrono::system_clock::now();

		auto readTime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
		auto interpTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		cout << "time to read: " << readTime << " time to interp: " << interpTime << endl;

		prevTime = timeI;
	}
	else cout << "Reused time " << prevTime << std::endl;

	//siirretään imagedata ulostuloon
	ds->DeepCopy(im);

	//kerrotaan mitä dataa löytyi
	ds->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(), dataInfo.Time().EpochTime());
	outInfo->Set(vtkDataObject::DATA_EXTENT(), reqExtent ? reqExtent : im->GetExtent(), 6);

	Modified();

	//ptime utcTime = ptime(from_time_t(dataInfo.Time().EpochTime()));

	//cout << "Returned time " << dataInfo.Time().EpochTime() << ", UTC: " << to_iso_extended_string( utcTime.date() ) <<", "<< utcTime.time_of_day() << endl;

	//ds->ReleaseData();

	return 1;
}
