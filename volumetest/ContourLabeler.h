#ifndef CONTOURLABELER_H
#define CONTOURLABELER_H

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkXMLPolyDataReader.h>
#include <vtkContourFilter.h>
#include <vtkStripper.h>
#include <vtkPolyDataMapper.h>
#include <vtkLabeledDataMapper.h>
#include <vtkPlaneSource.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkActor2D.h>

#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkSelectVisiblePoints.h>

class ContourLabeler {

	vtkSmartPointer<vtkLabeledDataMapper> labelMapper;
	vtkSmartPointer<vtkActor2D> labelActor;
	vtkSmartPointer<vtkSelectVisiblePoints> pointSelector;
	vtkSmartPointer<vtkPoints> points;
	vtkSmartPointer<vtkDoubleArray> labels;

	vtkSmartPointer<vtkPolyData> polyData;
public:
	ContourLabeler(vtkRenderer *ren) :
		labelMapper(vtkSmartPointer<vtkLabeledDataMapper>::New()),
		labelActor(vtkSmartPointer<vtkActor2D>::New()),
		pointSelector(vtkSmartPointer<vtkSelectVisiblePoints>::New()),
		points(vtkSmartPointer<vtkPoints>::New()),
		labels(vtkSmartPointer<vtkDoubleArray>::New()),
		polyData(vtkSmartPointer<vtkPolyData>::New())
	{

		labels->SetNumberOfComponents(1);
		labels->SetName("Isovalues");

		pointSelector->SetRenderer(ren);


		labelMapper->SetInputConnection(pointSelector->GetOutputPort());
		labelMapper->SetFieldDataName("Isovalues");
		labelMapper->SetLabelModeToLabelScalars();
		labelMapper->SetLabelFormat("%6.2f");


		labelActor->SetMapper(labelMapper );
		ren->AddActor( labelActor);
	}
	inline void Clear() {
		points->Reset();
		labels->Reset();
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
	}
};


#endif /* CONTOURLABELER_H */