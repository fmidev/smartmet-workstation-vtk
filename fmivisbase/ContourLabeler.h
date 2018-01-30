#ifndef CONTOURLABELER_H
#define CONTOURLABELER_H

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>

#include <vtkPolyDataMapper.h>
#include <vtkLabeledDataMapper.h>

#include <vtkActor2D.h>

#include <vtkPoints.h>
#include <vtkPointData.h>

#include <vtkDoubleArray.h>
#include <vtkSelectVisiblePoints.h>

#include "DrawOptions.h"

class ContourLabeler {

	vtkSmartPointer<vtkLabeledDataMapper> labelMapper;
	vtkSmartPointer<vtkActor2D> labelActor;
	vtkSmartPointer<vtkSelectVisiblePoints> pointSelector;
	vtkSmartPointer<vtkPoints> points;
	vtkSmartPointer<vtkDoubleArray> labels;

	vtkSmartPointer<vtkPolyData> polyData;

	vtkRenderer *ren;
public:
	ContourLabeler(vtkRenderer *ren) :
		labelMapper(vtkSmartPointer<vtkLabeledDataMapper>::New()),
		labelActor(vtkSmartPointer<vtkActor2D>::New()),
		pointSelector(vtkSmartPointer<vtkSelectVisiblePoints>::New()),
		points(vtkSmartPointer<vtkPoints>::New()),
		labels(vtkSmartPointer<vtkDoubleArray>::New()),
		polyData(vtkSmartPointer<vtkPolyData>::New()),
		ren(ren)
	{

		labels->SetNumberOfComponents(1);
		labels->SetName("Isovalues");

		pointSelector->SetRenderer(ren);

		labelMapper->SetInputConnection(pointSelector->GetOutputPort());
		labelMapper->SetFieldDataName("Isovalues");
		labelMapper->SetLabelModeToLabelScalars();
		labelMapper->SetLabelFormat("%6.2f");


		labelActor->SetMapper(labelMapper );
		
	}
	inline void Clear() {
		points->Reset();
		labels->Reset();
		ren->RemoveActor(labelActor);

	}
	inline void Add(double point[3], double label) {
		points->InsertNextPoint( point );
		labels->InsertNextTuple1( label );
	}
	inline void Update() {
		polyData->SetPoints(points);
		polyData->GetPointData()->SetScalars(labels);

		pointSelector->SetInputData(polyData);

		pointSelector->Update();
		labelMapper->Update();


		ren->AddActor(labelActor);
	}
};


#endif /* CONTOURLABELER_H */