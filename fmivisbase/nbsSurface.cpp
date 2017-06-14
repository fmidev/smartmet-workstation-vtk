#include "nbsSurface.h"

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkDelaunay2D.h>
#include <vtkFloatArray.h>

#include <future>

#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include <vtkDataArray.h>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>

#include "newBaseSourcer.h"
#include "nbsImpl.h"

#include "nbsMetadata.h"

struct nbsSurface::nbsImpl {

	NFmiQueryData data;
	NFmiFastQueryInfo dataInfo;
	nbsImpl(const std::string &file) : data(file), dataInfo(&data) {}
};


nbsSurface::nbsSurface(const std::string &file, nbsMetadata *meta, int param,int zHeight) :
	pimpl(std::make_unique <nbsImpl>(file)), param(param), meta(meta), zHeight(zHeight), prevTime(-1)
{

	SetNumberOfInputPorts(0);
	inputPolyData = vtkPolyData::New();
	delaunay = vtkDelaunay2D::New();
}

nbsSurface::~nbsSurface() {
	inputPolyData->Delete();
	delaunay->Delete();
}


int nbsSurface::RequestInformation(vtkInformation* vtkNotUsed(request),
	vtkInformationVector** vtkNotUsed(inputVector),
	vtkInformationVector* outputVector) {


	vtkInformation* outInfo = outputVector->GetInformationObject(0);


	double doubleRange[2];
	doubleRange[0] = meta->times.front();
	doubleRange[1] = meta->times.back();


	outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), doubleRange, 2);
	outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &meta->times[0], meta->times.size());

	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 0, meta->sizeX - 1, 0, meta->sizeY - 1, 0, 1);
	return 1;
}


int nbsSurface::nearestIndex(double time)
{
	for (auto iter = meta->timeIndex.begin(); iter != meta->timeIndex.end(); iter++) {
		if (iter->first == time) return iter->second;
		auto nextIter = iter;
		nextIter++;
		if (nextIter != meta->timeIndex.end() && nextIter->first > time) return iter->second;
	}
	if (meta->timeIndex.rbegin() != meta->timeIndex.rend()) return meta->timeIndex.rbegin()->second;

	return 0;
}



int nbsSurface::RequestData(vtkInformation* vtkNotUsed(request),
	vtkInformationVector** vtkNotUsed(inputVector),
	vtkInformationVector* outputVector) {


	int sizeX = meta->sizeX;
	int sizeY = meta->sizeY;
	int sizeZ = 1;


	auto points = vtkSmartPointer<vtkPoints>::New();


	auto textureCoordinates = vtkSmartPointer<vtkFloatArray>::New();
	textureCoordinates->SetNumberOfComponents(3);
	textureCoordinates->SetName("TextureCoordinates");


	unsigned long time = 0;

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

		dataInfo.ResetTime();

		float minVal = kMaxFloat, maxVal = kMinFloat;

		float maxHeight = -1;


		auto t0 = std::chrono::system_clock::now();


		if (!dataInfo.Param(kFmiGeopHeight)) {
			cout << "kFmiGeopHeight not found!" << endl;

			if (!dataInfo.Param(kFmiGeomHeight)) {
				cout << "kFmiGeomHeight not found!!" << endl;
			}
		}

		dataInfo.TimeIndex(timeI);
		int ix, iz = 0;

		bool rising = dataInfo.HeightParamIsRising();

		if (rising) dataInfo.ResetLevel();
		else dataInfo.LastLevel();

		float h;
		ix = 0;
		for (dataInfo.ResetLocation(); dataInfo.NextLocation(); ) {

			int x = ix % sizeX;
			int y = (ix / sizeX) % sizeY;

			float val = dataInfo.FloatValue();

			if (val == kFloatMissing) {
				val = 0;
			}

			points->InsertNextPoint(x * 2, y * 2, val * 2 / zHeight * 80); //VisualizerManager zHeight ja newBaseSourcer zRes, spacing


			float tuple[3] = { 0.0, 0.0, 0.0 };
			tuple[0] = float(x) / sizeX; tuple[1] = float(y) / sizeY; tuple[2] = 0.0;
			textureCoordinates->InsertNextTuple(tuple);


			ix++;
		}


		auto scalars = vtkSmartPointer<vtkFloatArray>::New();

		scalars->SetNumberOfComponents(1);
		scalars->SetName("Scalars");

		if (dataInfo.Param(FmiParameterName(param))) {



			for (dataInfo.ResetLocation(); dataInfo.NextLocation(); ) {

				int x = ix % sizeX;
				int y = (ix / sizeX) % sizeY;

				float val = dataInfo.FloatValue();

				if (val == kFloatMissing) {
					val = 0;
				}

				scalars->InsertNextTuple1(val);

				ix++;
			}
		}

		cout << "Triangulating... ";

		// Add the grid points to a polydata object
		inputPolyData->SetPoints(points);
		inputPolyData->GetPointData()->SetTCoords(textureCoordinates);
		inputPolyData->GetPointData()->SetScalars(scalars);
		inputPolyData->GetPointData()->SetActiveAttribute("Scalars", 0);

		// Triangulate the grid points
		delaunay->SetInputData(inputPolyData);

		//accuracy vs speed
		delaunay->SetTolerance(0.005);

		delaunay->Update();

		cout << "Done" << endl;

		prevTime = timeI;
	}
	else cout << "Reused time " << prevTime << std::endl;

	//siirretään imagedata ulostuloon
	ds->DeepCopy(delaunay->GetOutput() );

	//kerrotaan mitä dataa löytyi
	ds->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(), dataInfo.Time().EpochTime());

	Modified();


	return 1;
}