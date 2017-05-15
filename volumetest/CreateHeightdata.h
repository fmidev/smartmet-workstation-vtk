#ifndef CREATEHEIGHTDATA_H
#define CREATEHEIGHTDATA_H

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkDelaunay2D.h>
#include <vtkFloatArray.h>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>


inline vtkPolyData * CreateHeightdata(std::string file) {

	auto data = NFmiQueryData(file);
	auto dataInfo = NFmiFastQueryInfo(&data);


	auto points = vtkSmartPointer<vtkPoints>::New();


	auto textureCoordinates = vtkSmartPointer<vtkFloatArray>::New();
	textureCoordinates->SetNumberOfComponents(3);
	textureCoordinates->SetName("TextureCoordinates");


	auto sizeX = dataInfo.GridXNumber();
	auto sizeY = dataInfo.GridYNumber();

	if (dataInfo.Param(kFmiGeopHeight)) {

		dataInfo.TimeIndex(1);
		int ix, iz = 0;

		bool rising = dataInfo.HeightParamIsRising();

		if (rising) dataInfo.ResetLevel();
		else dataInfo.LastLevel();

		do {
			float h;
			ix = 0;
			for (dataInfo.ResetLocation(); dataInfo.NextLocation(); ) {

				int x = ix % sizeX;
				int y = (ix / sizeX) % sizeY;

				float val = dataInfo.FloatValue();

				if (val == kFloatMissing) {
					val = 0;
				}

				points->InsertNextPoint(x*2, y*2, val*2 / 13000 * 70); //VisualizerManager zHeight ja newBaseSourcer zRes
				

				float tuple[3] = { 0.0, 0.0, 0.0 };
				tuple[0] = float(x)/sizeX; tuple[1] = float(y) / sizeY; tuple[2] = 0.0;
				textureCoordinates->InsertNextTuple(tuple);


				ix++;
			}

			iz++;

		} while ((rising && dataInfo.NextLevel()) || (!rising && dataInfo.PreviousLevel()));

	}







	// Add the grid points to a polydata object
	static auto inputPolyData = vtkSmartPointer<vtkPolyData>::New();
	inputPolyData->SetPoints( points );
	inputPolyData->GetPointData()->SetTCoords(textureCoordinates);

	// Triangulate the grid points
	static auto delaunay = vtkSmartPointer<vtkDelaunay2D>::New();
	delaunay->SetInputData(inputPolyData);
	delaunay->Update();

	return delaunay->GetOutput();
}

#endif /* CREATEHEIGHTDATA_H */