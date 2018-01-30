#include "newBaseSourcer.h"

#include <future>

#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include <vtkImageData.h>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>

#include "nbsMetadata.h"

#include "nbsImpl.h"

newBaseSourcer::newBaseSourcer(const std::string &file, nbsMetadata *meta, int param, int res, int sub) :
	newBaseSourcerBase(file,meta,param,res),
	im(nullptr), heights(),
	subSample(sub)
{
	SetNumberOfInputPorts(0);

	ReadHeights(1);
}

const NFmiDataIdent &newBaseSourcer::getParamDescr()
{
	return pimpl->data.Param();
}

newBaseSourcer::~newBaseSourcer() {}

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

	int sizeX = meta->sizeX / subSample;
	int sizeY = meta->sizeY / subSample;
	int sizeZ = zRes / subSample;


	unsigned long time = 0;

	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	//pyydetäänkö tiettyä ajanjaksoa
	if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP())) {
		time = meta->timeStepToEpoch ( outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()) );
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

	NFmiFastQueryInfo &dataInfo = pimpl->dataInfo;

	//onko aika-askel jo muistissa
	if (timeI != prevTime) {

		ResetImage();

		dataInfo.ResetTime();

		float minVal = kMaxFloat, maxVal = kMinFloat;

		float maxHeight = -1;

		
		auto t0 = std::chrono::system_clock::now();

		//luetaan datapisteiden korkeudet


		//luetaan parametri
		LoopParam(param,timeI,[=,&maxVal,&minVal](int x, int y, int z, float val) {
					if (meta->hasHeight) {
						z = getHeight(x, y, z);
				
					}


					if (z >= sizeZ)
						z = sizeZ - 1;

						float* pixel = static_cast<float*>(im->GetScalarPointer(x, y, z));

						if (pixel[0] != kFloatMissing) {

							pixel[0] += val;


							if (pixel[0] > maxVal) maxVal = pixel[0];
							if (pixel[0] < minVal) minVal = pixel[0];
						}
						else
							pixel[0] = val;
				} );
		float magnitude = maxVal - minVal;

		cout << param << ": max: " << maxVal << " min: " << minVal << ", mag: " << magnitude << std::endl;

		auto t1 = std::chrono::system_clock::now();

		//for (int i = 0; i<zRes; ++i)
		//	cout << "z: " << i << " : " << static_cast<float*>(im->GetScalarPointer(0, 90, i))[0] << endl;

		//cout << "interpolating..." << endl;

		unsigned int usedThreadCount = boost::thread::hardware_concurrency();
		auto threads = std::list<std::future<void>>();

		int blockWidth = 3;

		for (int i = 0; i < blockWidth*blockWidth; ++i) {
			int blockX = i % blockWidth;
			int blockY = i / blockWidth;

			int startX = blockX*sizeX / blockWidth;
			int endX;

			if (blockX < blockWidth - 1) {

				endX = startX + sizeX / blockWidth;
			}
			else {
				endX = sizeX;
			}

			int startY = blockY*sizeY / blockWidth;
			int endY;

			if (blockY < blockWidth - 1) {
				endY = startY + sizeY / blockWidth;
			}
			else {
				endY = sizeY;
			}

			if (threads.size() >= usedThreadCount) {
				auto iter = threads.begin();
				while (iter != threads.end())
				{

					if (iter->valid() && iter->wait_for(std::chrono::milliseconds(1)) == std::future_status::ready) {
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
			}) );
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


int newBaseSourcer::getHeight(int x, int y, int z) {

	int sizeX = meta->sizeX / subSample;
	int sizeY = meta->sizeY / subSample;
	int sizeZ = meta->sizeZ / subSample;



	bool rising = pimpl->dataInfo.HeightParamIsRising();

	if (rising) z = sizeZ-1 - z;

	int ret = std::roundf(heights[z + x*sizeZ + y*sizeZ*sizeX]);

	return ret;
}


//heights is same dimensions as newbase is, subsampled
void newBaseSourcer::ReadHeights(int time) {


	int sizeX = meta->sizeX / subSample;
	int sizeY = meta->sizeY / subSample;
	int sizeZ = zRes / subSample;

	int dataZ = meta->sizeZ;

	auto &dataInfo = pimpl->dataInfo;

	//if(heights==nullptr AllocateHeights();


	if (dataInfo.Param(kFmiGeopHeight))

		dataInfo.GetCube(heights);
	else if (dataInfo.Param(kFmiGeomHeight)) {
		cout << "Warning: kFmiGeopHeight not found, using kFmiGeomHeight" << endl;

		dataInfo.GetCube(heights);
	}
	else {
		cout << "Warning: kFmiGeomHeight not found, heightdata unavailable" << endl;
		return;
	}

	std::transform(begin(heights), end(heights), begin(heights), [=](float val) {
		return val / zHeight * float(sizeZ);
	});

	dataZ += 0;

// 	auto loop = [&](int x, int y, int z, float val) {
// 
// 
// 		if (val == kFloatMissing) {
// 			val = 0;
// 			cout << "Missing height at " << x << ", " << y << ", " << z << endl;
// 			return false;
// 		}
// 
// 		float h = (val) / zHeight * float(sizeZ);
// 
// 		heights[x + y *sizeX + z*sizeX*sizeY] = h;
// 		return true;
// 	};

// 
// 	if (!LoopParam(kFmiGeopHeight, time, loop)) {
// 		cout << "Warning: kFmiGeopHeight not found, using kFmiGeomHeight" << endl;
// 		if (!LoopParam(kFmiGeomHeight, time, loop)) {
// 			cout << "Warning: kFmiGeomHeight not found, heightdata unavailable" << endl;
// 		}
// 	}
}

void newBaseSourcer::AllocateHeights() {
	int sizeX = meta->sizeX / subSample;
	int sizeY = meta->sizeY / subSample;
	int sizeZ = zRes / subSample;

	int dataZ = meta->sizeZ;

	auto &dataInfo = pimpl->dataInfo;

	//if (heights != nullptr) delete heights;

	//heights = new float[sizeX*sizeY*dataZ];

	heights.resize(sizeX*sizeY*dataZ);
}

void newBaseSourcer::ResetImage(bool realloc)
{

	int sizeX = meta->sizeX/subSample, sizeY = meta->sizeY / subSample, sizeZ = zRes/subSample;

	if (realloc || !im) {
		if (!im)
			im = vtkImageData::New();

		im->Initialize();
		im->SetDimensions(sizeX, sizeY, sizeZ);
		im->SetSpacing(2 * subSample, 2 * subSample, 2 * subSample);
		im->AllocateScalars(VTK_FLOAT, 1);
	}

	float* p = static_cast<float*>(im->GetScalarPointer());
	for (long iz = 0; iz < sizeZ; ++iz) {
		for (long iy = 0; iy < sizeY; ++iy) {
			for (long ix = 0; ix < sizeX; ++ix) {
				p[ix + iy*sizeX + iz*sizeX*sizeY] = kFloatMissing;
			}
		}
	}
}
