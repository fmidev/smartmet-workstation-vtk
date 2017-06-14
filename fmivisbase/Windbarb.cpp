#include "Windbarb.h"

#include <list>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkAppendPolyData.h>
#include <vtkLineSource.h>
#include <vtkTubeFilter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkCleanPolyData.h>

#include <vtkGlyph3D.h>

//windbarb for val in m/s
vtkSmartPointer<vtkPolyData> CreateWindbarb(float val, float size) {

	auto append = vtkAppendPolyData::New();

	auto seed = vtkLineSource::New();
	seed->SetPoint1(0, 0, 0);
	seed->SetPoint2(0, 0, 1.5);
	seed->Update();


	auto body = vtkTubeFilter::New();
	body->SetRadius(0.1);

	body->SetInputConnection(seed->GetOutputPort());
	body->CappingOn();
	body->SetNumberOfSides(5);

	body->SetVaryRadiusToVaryRadiusOff();
	body->Update();


	auto dataList = std::list< vtkSmartPointer< vtkPolyData > >{};


	auto data = vtkSmartPointer<vtkPolyData>::New();

	data->ShallowCopy(body->GetOutput());
	append->AddInputData(data);
	dataList.push_back(data);

	double z = 0;

	val *= 0.5f; // m/s to knots

	while (val >= 5.0f) {
		if (val >= 50.0f) {
			z += 0.2f;

			seed->SetPoint1(0.08, 0, z-0.12);
			seed->SetPoint2(0.5, 0, z);


			body->SetRadius(0.075);
			body->Update();

			data = vtkSmartPointer<vtkPolyData>::New();
			data->ShallowCopy(body->GetOutput());

			append->AddInputData(data);
			dataList.push_back(data);


			seed->SetPoint1(0.08, 0, z + 0.12);
			seed->SetPoint2(0.5, 0, z);

			z += 0.1f;

			body->SetRadius(0.075);
			body->Update();

			data = vtkSmartPointer<vtkPolyData>::New();
			data->ShallowCopy(body->GetOutput());

			append->AddInputData(data);
			dataList.push_back(data);
			val -= 50.0f;
		}
		else if (val >= 10.0f) {
			z += 0.125f;
			seed->SetPoint1(0.1, 0, z);
			seed->SetPoint2(0.5, 0, z);

			body->SetRadius(0.05);
			body->Update();

			data = vtkSmartPointer<vtkPolyData>::New();
			data->ShallowCopy(body->GetOutput());

			append->AddInputData(data);
			dataList.push_back(data);
			val -= 10.0f;
		}
		else {
			z += 0.12f;
			seed->SetPoint1(0.1, 0, z);
			seed->SetPoint2(0.3,0, z);

			body->SetRadius(0.04);
			body->Update();

			data = vtkSmartPointer<vtkPolyData>::New();
			data->ShallowCopy(body->GetOutput());

			append->AddInputData(data);
			dataList.push_back(data);
			val -= 5.0f;
		}

	}

	append->Update();

	auto cleanFilter = vtkCleanPolyData::New();
	cleanFilter->SetInputConnection(append->GetOutputPort() );
	cleanFilter->Update();

	auto transform = vtkTransform::New();

	transform->RotateY(90);
	transform->Scale(size, size, size);

	auto transformFilter = vtkTransformPolyDataFilter::New();
	transformFilter->SetInputData(append->GetOutput());

	transformFilter->SetTransform(transform);

	transformFilter->Update();


	auto ret = vtkSmartPointer<vtkPolyData>::New();

	ret->ShallowCopy(transformFilter->GetOutput());

	append->Delete();
	body->Delete();
	seed->Delete();

	transform->Delete();
	cleanFilter->Delete();
	transformFilter->Delete();

	

	return ret;
}

void GenerateBarbs(vtkGlyph3D *glyph,float size) {
	for (int i = 0; i < 30; ++i) glyph->SetSourceData(i, CreateWindbarb(i * 5, size));
	glyph->SetRange(0, 80);
}