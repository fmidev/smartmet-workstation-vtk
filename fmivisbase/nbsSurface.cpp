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

#include "nbsMetadata.h"




nbsSurface::nbsSurface(const std::string &file, nbsMetadata *meta, int param, int zHeight, bool flat, bool pad) :
	newBaseSourcerBase(file, meta, param,1), flat(flat), pad(pad), spacing(2)
{
	if (param == 0) throw new std::invalid_argument("param is 0");
	SetNumberOfInputPorts(0);
	inputPolyData = vtkPolyData::New();

	points = vtkSmartPointer<vtkPoints>::New();

	textureCoordinates = vtkSmartPointer<vtkFloatArray>::New();
	textureCoordinates->SetNumberOfComponents(2);
	textureCoordinates->SetName("TextureCoordinates");

	scalars = vtkSmartPointer<vtkFloatArray>::New();

	scalars->SetNumberOfComponents(1);
	scalars->SetName("Scalars");

	normals = vtkSmartPointer<vtkFloatArray>::New();

	normals->SetNumberOfComponents(3);
	normals->SetName("Normals");

	loadPoints();
}

nbsSurface::~nbsSurface() {
	inputPolyData->Delete();

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


bool nbsSurface::loadPoints() {



	int sizeX = meta->sizeX;
	int sizeY = meta->sizeY;
	int sizeZ = 1;

	int gridX = sizeX;
	int gridY = sizeY;

	if (pad) {
		gridX += 2; 
		gridY += 2;
	}

	NFmiFastQueryInfo &dataInfo = pimpl->dataInfo;


	points->Resize(gridX*gridY);

	if (flat) {


		float normal[3] = { 0,0,1 };

		for (int x = 0; x < gridX; ++x)
			for (int y = 0; y < gridY; ++y)
			{
				points->InsertNextPoint(x * spacing - (pad ? spacing : 0),
					y * spacing - (pad ? spacing : 0), 0.1);

				float tuple[2] = { std::min((float)x / gridX, 1.0f),
					std::min((float)y / gridY, 1.0f) + 0 };

				textureCoordinates->InsertNextTuple(tuple);

				normals->InsertNextTuple(normal);
			}
	}
	else {

		if (!dataInfo.Param(kFmiGeopHeight)) {
			cout << "kFmiGeopHeight not found!" << endl;

			if (!dataInfo.Param(kFmiGeomHeight)) {
				cout << "kFmiGeomHeight not found!!" << endl;
				return false;
			}
		}


		static std::vector<float> values;

		if (dataInfo.HeightParamIsRising())
			dataInfo.FirstLevel();
		else 
			dataInfo.LastLevel();

		dataInfo.GetLevelToVec(values);


		for (int x = 0; x < gridX; ++x)
			for (int y = 0; y < gridY; ++y)
			{
				points->InsertNextPoint(x * spacing - ( pad ? spacing : 0),
										y * spacing - (pad ? spacing : 0),
									values[x + y*sizeX] * spacing / zHeight * 80); //VisualizerManager zHeight ja newBaseSourcer zRes


				float tuple[2] = { std::min((float)x / gridX, 1.0f),
					std::min((float)y / gridY, 1.0f) + 0 };

				textureCoordinates->InsertNextTuple(tuple);
			}
	}

	cout << "Triangulating... ";

	inputPolyData->SetPoints(points);

	static vtkSmartPointer<vtkDelaunay2D> delaunay = nullptr;
	if (!delaunay) {

		delaunay = vtkSmartPointer<vtkDelaunay2D>::New();

		// Triangulate the grid points
		delaunay->SetInputData(inputPolyData);

		//accuracy vs speed
		delaunay->SetTolerance(0.005);

		delaunay->Update();
	}

	// Add the grid points to a polydata object
	inputPolyData->ShallowCopy(delaunay->GetOutput());

	inputPolyData->GetPointData()->SetTCoords(textureCoordinates);
	if(flat) inputPolyData->GetPointData()->SetNormals(normals);

	return true;
}

bool extentIncreased(int* reqExtent, int* prevExtent) {

	return (reqExtent[0] < prevExtent[0])
		|| (reqExtent[1] > prevExtent[1])
		|| (reqExtent[2] < prevExtent[2])
		|| (reqExtent[3] > prevExtent[3]);
}

int nbsSurface::RequestData(vtkInformation* vtkNotUsed(request),
	vtkInformationVector** vtkNotUsed(inputVector),
	vtkInformationVector* outputVector) {


	int sizeX = meta->sizeX;
	int sizeY = meta->sizeY;
	int sizeZ = 1;


	int gridX = sizeX;
	int gridY = sizeY;

	if (pad) {
		gridX += 2;
		gridY += 2;
	}

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
	if (timeI != prevTime || extentIncreased(reqExtent,prevExtent)  ) {

		dataInfo.ResetTime();

		float minVal = kMaxFloat, maxVal = kMinFloat;

		float maxHeight = -1;


		dataInfo.TimeIndex(timeI);
		auto t0 = std::chrono::system_clock::now();


		static std::vector<float> values;

		scalars->Reset();


		if (dataInfo.Param(FmiParameterName(param))) {

			if (dataInfo.HeightParamIsRising()) dataInfo.FirstLevel();
			else dataInfo.LastLevel();

			int eWidth = reqExtent[1] - reqExtent[0]+1;
			int eHeight = reqExtent[3] - reqExtent[2]+1;

			float shrinkRatio = float(eWidth)*float(eHeight) / float(sizeX*sizeY);

			//if (shrinkRatio > 0.8)
			//	dataInfo.GetLevelToVec(values);
			//else
				dataInfo.GetLevelToVecPartial(reqExtent[0], reqExtent[2], reqExtent[1], reqExtent[3], values);

			for (int x = 0; x < sizeX; ++x)
				for (int y = 0; y < sizeY; ++y)
				{

					float val = kFloatMissing;

					if(x>=reqExtent[0] && x<reqExtent[1]
					&& y>=reqExtent[2] && y<reqExtent[3])
					{
						int valX = x - reqExtent[0];
						int valY = y - reqExtent[2];

						if (pad) {

							if ((x == 0 || x == gridX - 3)
								|| (y == 0 || y == gridY - 3))
								val = kFloatMissing;
							else {
								val = values[valX + (valY)*eWidth];
							}

						}
						else
							val = values[valX + valY*eWidth];
					}

					if (val == kFloatMissing) {
						val = 0;
					}

					scalars->InsertNextTuple1(val);

				}
		}
		else throw new std::runtime_error("No param found");



		inputPolyData->GetPointData()->SetScalars(scalars);
		inputPolyData->GetPointData()->SetActiveAttribute("Scalars", 0);

		cout << "Done" << endl;

		prevTime = timeI;
		for (int i = 0; i < 6; i++)
			prevExtent[i] = reqExtent[i];


	}
	else cout << "Reused time " << prevTime << std::endl;

	//siirretään imagedata ulostuloon
	ds->ShallowCopy( inputPolyData );
	//kerrotaan mitä dataa löytyi
	ds->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(), dataInfo.Time().EpochTime());


	Modified();

	return 1;
}