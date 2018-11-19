#include "ParamVisualizerArrayText.h"

#include <vtkTextActor3D.h>
#include <vtkPropAssembly.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkTextProperty.h>
#include <vtkScalarsToColors.h>

#include "nbsSurface.h"

namespace fmiVis {

	ParamVisualizerArrayText::ParamVisualizerArrayText(const std::string &file, nbsMetadata &m,
		NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac, vtkSmartPointer<vtkScalarsToColors> col) :
		ParamVisualizerBase(new nbsSurface(file, &m, paramIdent.GetParamIdent(), 13000, true), m, paramIdent, fac),
		acts(),
		assembly(vtkSmartPointer<vtkPropAssembly>::New()),
		colF(col)
	{
		nbs->Update();

		auto sizeX = m.sizeX;
		auto sizeY = m.sizeY;


		for (int iy = 0; iy < sizeY; iy += 5)
			for (int ix = 0; ix < sizeX; ix += 5)
			{
				auto act = vtkTextActor3D::New();
				act->SetPosition(3 + ix * 2, 3 + iy * 2, 1);
				act->GetTextProperty()->SetFontSize(50);
				act->GetTextProperty()->SetJustificationToCentered();
				act->GetTextProperty()->SetFontFamilyToCourier();
				act->GetTextProperty()->SetColor(255, 255, 255);
				//act->GetTextProperty()->ShadowOn();
				act->SetScale(0.12);
				acts.push_back(act);
				assembly->AddPart(act);
			}

		SetProp(assembly);
	}


	ParamVisualizerArrayText::~ParamVisualizerArrayText()
	{
		for (auto act : acts) {
			assembly->RemovePart(act);
			act->Delete();
		}
	}

	void ParamVisualizerArrayText::UpdateTimeStep(double t)
	{

		int sizeX = meta.sizeX;
		int sizeY = meta.sizeY;

		UpdateNBS(t);

		vtkPolyData * input = vtkPolyData::SafeDownCast(nbs->GetOutputDataObject(0));


		auto inputSize = input->GetNumberOfPoints();

		auto inputScalars = input->GetPointData()->GetScalars();


		int ix = 0, iy = 0;
		int stepX = 5, stepY = 5;


		while (iy < sizeY) {
			ix = 0;
			while (ix < sizeX) {

				int i = iy + sizeY * ix;


				auto val = inputScalars->GetTuple1(i);


				auto s = std::ostringstream{};

				s << val;

				auto ta = acts[ix / 5 + iy / 5 * sizeX / 5];

				if (colF) {
					double col[3];
					colF->GetColor(val, col);
					ta->GetTextProperty()->SetColor(col);
				}

				ta->SetInput(s.str().c_str());

				ix += stepX;
			}
			iy += stepY;

		}
	}

}