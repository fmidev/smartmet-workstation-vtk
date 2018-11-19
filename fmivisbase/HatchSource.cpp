#include "HatchSource.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkPoints.h>
#include <vtkLine.h>
#include <vtkIdList.h>
#include <vtkCellArray.h>

namespace fmiVis {

	int HatchSource::RequestData(vtkInformation* vtkNotUsed(request), vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
	{
		vtkInformation* outInfo = outputVector->GetInformationObject(0);
		vtkDataObject *ds = outInfo->Get(vtkDataSet::DATA_OBJECT());


		ds->ShallowCopy(data);

		return 1;
	}

	void HatchSource::Generate() {
		data->Initialize();

		auto points = vtkSmartPointer<vtkPoints>::New();
		auto lines = vtkSmartPointer<vtkCellArray>::New();

		for (int i = 1; i < density * 2; ++i) {

			double p1[3] = { 0,0,1 };
			double p2[3] = { 0,0,1 };

			if (i <= density) {

				p1[0] = double(i) / density * width;
				p2[1] = double(i) / density * height;
			}
			else {
				int j = density * 2 - i;

				p1[0] = double(j) / density * width;
				p1[1] = height;

				p2[0] = width;
				p2[1] = double(j) / density * height;
			}

			points->InsertNextPoint(p1);
			points->InsertNextPoint(p2);

			auto line = vtkSmartPointer<vtkLine>::New();

			line->GetPointIds()->SetId(0, 2 * i - 2);
			line->GetPointIds()->SetId(1, 2 * i - 1);

			lines->InsertNextCell(line);

		}


		data->SetPoints(points);
		data->SetLines(lines);

		Modified();

	}
	HatchSource::HatchSource() :
		width(0), height(0), density(0), data(vtkPolyData::New())
	{
		SetNumberOfInputPorts(0);
	}

	HatchSource::~HatchSource() {
		data->Delete();
	}

}