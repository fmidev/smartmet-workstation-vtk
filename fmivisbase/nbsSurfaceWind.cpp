#include "nbsSurfaceWind.h"

#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>

#include "newBaseSourcer.h"
#include "nbsSurfaceImpl.h"

#include <chrono>

#include "nbsMetadata.h"


nbsSurfaceWind::nbsSurfaceWind(const std::string &file, nbsMetadata *meta, int subSample) :
	nbsSurface(file,meta,1,true), subSample(subSample)
{
	scalars->SetNumberOfComponents(3);
	scalars->SetName("Scalars");
	scalars->Resize(meta->sizeX*meta->sizeY);
}

nbsSurfaceWind::~nbsSurfaceWind() {}


int nbsSurfaceWind::RequestData(vtkInformation* vtkNotUsed(request),
	vtkInformationVector** vtkNotUsed(inputVector),
	vtkInformationVector* outputVector) {


	int sizeX = meta->sizeX / subSample;
	int sizeY = meta->sizeY / subSample;
	int sizeZ = 1;

	unsigned long time = 0;


	auto t0 = std::chrono::system_clock::now();

	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	//pyydetäänkö tiettyä ajanjaksoa
	if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP())) {
		time = meta->timeStepToEpoch(outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()));
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


		dataInfo.TimeIndex(timeI);

		float minVal = kMaxFloat, maxVal = kMinFloat;

		static auto speedValues = std::vector<float>();
		static auto dirValues = std::vector<float>();

		dataInfo.Param(kFmiWindSpeedMS);
		dataInfo.GetLevelToVec(speedValues);

		dataInfo.Param(kFmiWindDirection);
		dataInfo.GetLevelToVec(dirValues);


		scalars->Reset();

		for (int ix = 0; ix < sizeX; ++ix)
			for (int iy = 0; iy < sizeY; ++iy) {

			float val = dirValues[ix + iy*sizeX];


			val += 90;



			if (val != kFloatMissing) {

				NFmiAngle angle(dataInfo.Area()->TrueNorthAzimuth(dataInfo.LatLon()));
				val += angle.Value();

				if (val > 360.0f) val -= 360.0f;
				if (val < 0.0f) val += 360.0f;


				const float degToRad = 3.14159265358979f / 180.0f;

				float speed = speedValues[ix + iy*sizeX];
				if (speed == kFloatMissing) speed = 1;

				scalars->InsertNextTuple3(speed * std::cos(val*degToRad), speed * std::sin(val*degToRad),0 );

			}
			else {
				scalars->InsertNextTuple3(0, 0,0);
			}
		}

		// 		for (int i = 0; i < sizeZ; ++i) {
		// 			float *x = static_cast<float*>(im->GetScalarPointer(6, 6, i));
		// 			cout << "z: " << i << " : " << x[0] <<','<< x[1]<<','<< x[2] << endl;
		// 		}


		inputPolyData->GetPointData()->SetScalars(scalars);
		inputPolyData->GetPointData()->SetVectors(scalars);
		auto t1 = std::chrono::system_clock::now();

		auto readTime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
		cout << "time to read: " << readTime << endl;

		prevTime = timeI;
	}
	else cout << "Reused time " << prevTime << std::endl;
	//siirretään imagedata ulostuloon
	ds->ShallowCopy(inputPolyData);

	//kerrotaan mitä dataa löytyi
	ds->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(), dataInfo.Time().EpochTime());

	Modified();

	return 1;
}