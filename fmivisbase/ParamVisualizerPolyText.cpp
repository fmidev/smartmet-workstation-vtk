 #include "ParamVisualizerPolyText.h"

#include <vtkCellData.h>
#include <vtkVectorText.h>
#include <vtkPolyDataMapper.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkActor.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkFloatArray.h>

#include "nbsSurface.h"

ParamVisualizerPolyText::ParamVisualizerPolyText(const std::string &file, nbsMetadata &m, int param) :
	ParamVisualizerBase(new nbsSurface(file, &m, param, 13000, true), m, param),
	vt(vtkVectorText::New()),
	tf(vtkTransformPolyDataFilter::New()),
	ap(vtkAppendPolyData::New()),
	map(vtkPolyDataMapper::New()) ,
	act(vtkActor::New())
{
	nbs->Update();

	map->AddInputConnection(ap->GetOutputPort());

	map->SetScalarModeToUseCellData();
	map->SetColorModeToMapScalars();

	act->SetMapper(map);


	SetActiveMapper(act->GetMapper());

	SetProp(act);

}

ParamVisualizerPolyText::~ParamVisualizerPolyText()
{
	act->Delete();
	map->Delete();
	ap->Delete();
	vt->Delete();
}

void ParamVisualizerPolyText::UpdateTimeStep(double t)
{

	int sizeX = meta.sizeX;
	int sizeY = meta.sizeY;

	nbs->UpdateTimeStep(t);
	
	vtkPolyData * input = vtkPolyData::SafeDownCast(nbs->GetOutputDataObject(0));


	auto inputSize = input->GetNumberOfPoints();

	auto inputScalars = input->GetPointData()->GetScalars();

	auto pos = vtkSmartPointer<vtkTransform>::New();

	ap->RemoveAllInputs();

	int ix = 0, iy = 0;
	int step = 3;

	while (iy < sizeY) {
		ix = 0;
		while (ix < sizeX) {

			int i = iy + sizeY*ix;

			auto s = std::ostringstream{};

			auto val = inputScalars->GetTuple1(i);

			s << val;

			vt->SetText(s.str().c_str());
			vt->Update();

			double v[3];

			input->GetPoint(i, v);

			pos->Identity();


			pos->Translate(v);
			pos->Translate(0,0,2);
			pos->Scale(step*0.75, step*0.75, 1);

			tf->SetTransform(pos);


			tf->SetInputData(vt->GetOutput());
			tf->Update();

			auto data = vtkSmartPointer<vtkPolyData>::New();

			auto scalar = vtkSmartPointer<vtkFloatArray>::New();

			scalar->SetNumberOfComponents(1);

			for(int i=0;i<tf->GetOutput()->GetNumberOfCells();i++)
				scalar->InsertNextTuple1(val / 100);

			data->ShallowCopy(tf->GetOutput());
			data->GetCellData()->SetScalars(scalar);

			ap->AddInputData(data);
			ix += step;
		}
		iy += step;
	}

	ap->Update();

	map->Update();
}
