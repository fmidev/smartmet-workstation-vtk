#include "newBaseSourcer.h"

#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

void newBaseSourcer::initMeta() {
		meta->hasHeight = false;

		if (dataInfo.Area()) {
			const NFmiArea *a = dataInfo.Area();
			meta->p1 = a->BottomLeftLatLon();
			meta->p2 = a->TopRightLatLon();

		}

		meta->sizeX = dataInfo.GridXNumber();
		meta->sizeY = dataInfo.GridYNumber();
		meta->sizeZ = dataInfo.SizeLevels();
		int sizeX = meta->sizeX;
		int sizeY = meta->sizeX;
		int sizeZ = meta->sizeX;

		float minHeight = kMaxFloat, maxHeight = kMinFloat;


		dataInfo.ResetTime();
		dataInfo.NextTime();

		if (dataInfo.Param(kFmiGeopHeight)) {

			meta->hasHeight = true;

			bool rising = dataInfo.HeightParamIsRising();

			if (rising) dataInfo.ResetLevel();
			else dataInfo.LastLevel();

			do {

				for (dataInfo.ResetLocation(); dataInfo.NextLocation(); ) {
					float val = dataInfo.FloatValue();

					if (val == kFloatMissing) val = 0;
					if (val > maxHeight) maxHeight = val;
					if (val < minHeight) minHeight = val;

				}

			} while ((rising && dataInfo.NextLevel()) || (!rising && dataInfo.PreviousLevel()));
		}

		meta->minH = minHeight*0.99f;
		meta->maxH = maxHeight*1.01f;

		cout << "minheight: " << minHeight*0.99f << ", maxheight: " << maxHeight*1.01f << std::endl;

	}

	int newBaseSourcer::RequestInformation(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector) {

		if (times.empty()) {


			int n = 0;

			for (dataInfo.ResetTime(); dataInfo.NextTime(); ) {
				times.push_back(dataInfo.Time().EpochTime());
				timeIndex.insert({ dataInfo.Time().EpochTime(),n });
				n++;
			}

		}

		//cout << "start time: " << minT() << ", double conversion: " << minDT() << std::endl;
		//cout << "end time: " << maxT()<< ", double conversion: " << maxDT() << std::endl;



		vtkInformation* outInfo = outputVector->GetInformationObject(0);



		std::vector<double> doubleTimes;
		for (auto i = times.begin(); i != times.end(); i++) {
			doubleTimes.push_back(epochToDouble(*i));
		}

		double doubleRange[2];
		doubleRange[0] = doubleTimes.front();
		doubleRange[1] = doubleTimes.back();

		outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), doubleRange, 2);
		outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &doubleTimes[0], doubleTimes.size());


		outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 0, meta->sizeX - 1, 0, meta->sizeY - 1, 0, meta->sizeZ - 1);

		return 1;
}


int newBaseSourcer::RequestData(vtkInformation* vtkNotUsed(request),
	vtkInformationVector** vtkNotUsed(inputVector),
	vtkInformationVector* outputVector) {

	//pystyakseli vaikuttaa alikäytetyltä datassa
	float zScale = 0.5f;

	int sizeX = meta->sizeX;
	int sizeY = meta->sizeY;
	int sizeZ = meta->sizeZ*zScale;


	if (!im) {
		im = vtkImageData::New();
		im->Initialize();
		im->SetDimensions(sizeX, sizeY, sizeZ);
		im->SetSpacing(1, 1, 1 / zScale);
		im->AllocateScalars(VTK_FLOAT, 1);
	}

	unsigned long time = 0;

	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	//pyydetäänkö tiettyä ajanjaksoa
	if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP())) {
		time = doubleToEpoch(outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()));
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

		//nollataan kuva (ei välitä aluepyynnöistä)
		float* pixels = static_cast<float*>(im->GetScalarPointer());
		memset(pixels, 0.0f, sizeX*sizeY*sizeZ * sizeof(float)); //clears entire image!!


		dataInfo.ResetTime();

		float minVal = kMaxFloat, maxVal = kMinFloat;

		bool rising = dataInfo.HeightParamIsRising();

		heights;
		if (heights == nullptr) {
			heights = new float[sizeX*sizeY*sizeZ];
		}

		float maxHeight = -1;


		//luetaan datapisteiden korkeudet
		if (dataInfo.Param(kFmiGeopHeight)) {

			bool rising = dataInfo.HeightParamIsRising();

			dataInfo.TimeIndex(timeI);
			int ix, iz = 0;

			if (rising) dataInfo.ResetLevel();
			else dataInfo.LastLevel();

			do {
				ix = 0;
				for (dataInfo.ResetLocation(); dataInfo.NextLocation(); ) {

					int x = ix % sizeX;
					int y = (ix / sizeX) % sizeY;
					int z = iz*zScale;

					float val = dataInfo.FloatValue();



					if (val == kFloatMissing) val = 0;

					if (val > maxHeight)
						maxHeight = val;

					float h = (val - meta->minH) / meta->maxH * sizeZ;
					heights[x + y *sizeX + z*sizeX*sizeY] = h;

					ix++;
				}
				iz++;

			} while ((rising && dataInfo.NextLevel()) || (!rising && dataInfo.PreviousLevel()));

		}

		cout << "MaxHeight for time " << timeI << " is " << maxHeight << std::endl;

		int highest = -1;

		//luetaan parametri
		if (dataInfo.Param(FmiParameterName(param))) {

			dataInfo.TimeIndex(timeI);
			int ix, iz = 0;

			if (rising) dataInfo.ResetLevel();
			else dataInfo.LastLevel();

			do {

				ix = 0;
				for (dataInfo.ResetLocation(); dataInfo.NextLocation(); ) {
					int x = ix % sizeX;
					int y = (ix / sizeX) % sizeY;
					int z = iz*zScale;
					float zFrac = 1.0f;
					if (meta->hasHeight) {
						float modZ;
						zFrac = modf(heights[x + y*sizeX + z*sizeX*sizeY], &modZ);
						z = modZ;
					}
					if (reqExtent) {
						if (x<reqExtent[0] || x>reqExtent[1]
							|| y<reqExtent[2] || y>reqExtent[3]
							|| z<reqExtent[4] || z>reqExtent[5]) {
							ix++;
							continue;
						}

					}

					float val = dataInfo.FloatValue();
					if (val == kFloatMissing) val = 0; //voi myös piirtää laittamalla negatiiviseksi tjms
					if (val > maxVal) maxVal = val;
					if (val < minVal) minVal = val;

					if (val > 0.001f) {
						highest = z + 1;

						float* pixel = static_cast<float*>(im->GetScalarPointer(x, y, z));
						float* nextPixel = nullptr;
						if (z + 1 < sizeZ) static_cast<float*>(im->GetScalarPointer(x, y, z + 1));


						pixel[0] += val*(1.0f - zFrac);
						if (pixel[0] > maxVal) maxVal = pixel[0];
						if (nextPixel) nextPixel[0] += val*zFrac;
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



		prevTime = timeI;
	}
	else cout << "Reused time " << prevTime << std::endl;

	cout << "released: " << im->GetDataReleased() << std::endl;

	//siirretään imagedata ulostuloon
	ds->DeepCopy(im);

	//kerrotaan mitä dataa löytyi
	ds->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(), epochToDouble(dataInfo.Time().EpochTime()));
	outInfo->Set(vtkDataObject::DATA_EXTENT(), reqExtent ? reqExtent : im->GetExtent(), 6);

	//cout << "Returned time " << dataInfo.Time().EpochTime() << " , double: "<< epochToDouble( dataInfo.Time().EpochTime() ) << std::endl;

	//ds->ReleaseData();

	return 1;
}
