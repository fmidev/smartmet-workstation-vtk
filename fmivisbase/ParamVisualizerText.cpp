#include "ParamVisualizerText.h"

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
#include <vtkTexture.h>

#include <NFmiDataIdent.h>

#include "nbsSurface.h"
#include "TextImageLayer.h"
#include "vtkScalarsToColors.h"

namespace fmiVis {

	ParamVisualizerText::ParamVisualizerText(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac) :
		ParamVisualizerBase(new nbsSurface(file, &m, paramIdent.GetParamIdent(), 13000, true), m, paramIdent, fac),
		tl(1024, 1024),
		tf(vtkSmartPointer<vtkTransformPolyDataFilter>::New()),
		ap(vtkSmartPointer<vtkAppendPolyData>::New())
	{
		nbs->Update();

		map = vtkSmartPointer<vtkPolyDataMapper>::New();
		map->SetInputConnection(nbs->GetOutputPort());
		map->SetScalarVisibility(false);
		map->Update();

		texture = vtkSmartPointer<vtkTexture>::New();

		texture->SetInterpolate(true);


		act = vtkSmartPointer<vtkActor>::New();

		act->SetMapper(map);
		act->SetTexture(texture);
		act->SetPickable(false);
		act->SetDragable(false);

		act->SetMapper(map);

		tl.SetSize(25);

		SetActiveMapper(act->GetMapper());

		SetProp(act);

	}


	void ParamVisualizerText::UpdateTimeStep(double t)
	{

		int sizeX = meta.sizeX;
		int sizeY = meta.sizeY;

		UpdateNBS(t);

		vtkPolyData * input = vtkPolyData::SafeDownCast(nbs->GetOutputDataObject(0));


		auto inputSize = input->GetNumberOfPoints();

		auto inputScalars = input->GetPointData()->GetScalars();

		auto pos = vtkSmartPointer<vtkTransform>::New();


		int ix = 0, iy = sizeY - 1;
		int stepX = 5, stepY = 5;

		auto s = std::ostringstream{};

		while (iy >= 0) {
			ix = 0;
			while (ix < sizeX) {

				int i = iy + sizeY * ix;


				auto val = inputScalars->GetTuple1(i);

				auto len = 1 + floor(log10(val));
				if (val <= 1) len = 1;
				s << val;
				for (int i = 0; i < 4 - len; i++)
					s << " ";

				//double col[3] = { 0,0,0 };

				//map->GetLookupTable()->GetColor(val/100.0, col);

				//tl.SetColor(col[0], col[1],col[2]);


				ix += stepX;
			}
			iy -= stepY;
			if (iy >= 0)
				s << "\n";
		}
		tl.AddText(s.str(), 0, 0);
		texture->SetInputData(tl.GetImage());
		map->Update();
		tl.Clear();
	}

}