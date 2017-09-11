#include "ParamVisualizerIcon.h"

#include "vtkSmartPointer.h"

#include <vtkFloatArray.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkType.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkTexture.h>
#include <vtkIconGlyphFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProgrammableAttributeDataFilter.h>
#include <vtkPNGReader.h>


#include "nbsSurface.h"
#include "vtkSelectionNode.h"
#include "vtkSelection.h"
#include "vtkExtractSelectedIds.h"
#include "vtkGeometryFilter.h"

void AttributeCallback(void* arg) {

	auto vis = static_cast<ParamVisualizerIcon *>(arg);

	auto filter = vis->attribs;

	vtkPolyData * input = vtkPolyData::SafeDownCast(filter->GetInput());


	auto inputSize = input->GetNumberOfPoints();

	auto inputScalars = input->GetPointData()->GetScalars();

	auto index = vtkSmartPointer<vtkIntArray>::New();

	index->SetNumberOfTuples(inputSize);
	index->SetNumberOfComponents(1);
	index->SetName("IconIndex");

	for (int i = 0; i < inputSize; ++i)
		index->SetValue(i, vis->mapping(  *inputScalars->GetTuple(i)  ) );


	vtkPolyData * output = filter->GetPolyDataOutput();

	output->GetPointData()->CopyScalarsOff();
	output->GetPointData()->CopyTCoordsOff();
	output->GetPointData()->PassData(input->GetPointData());
	output->GetPointData()->SetScalars(index);

}

ParamVisualizerIcon::ParamVisualizerIcon(const std::string & file, nbsMetadata & m, int param, iconMapping mapping) :
ParamVisualizerBase(new nbsSurface(file, &m, param, 13000, true), m, param), mapping(mapping)
{
	nbs->Update();

	attribs = vtkSmartPointer<vtkProgrammableAttributeDataFilter>::New();
	attribs->SetInputConnection(nbs->GetOutputPort());

	attribs->SetExecuteMethod(&AttributeCallback, this);


	auto ids = vtkSmartPointer<vtkIdTypeArray>::New();
	ids->SetNumberOfComponents(1);

	auto width = 100;

	auto downScale = 3;


	// Set values

	for (unsigned int iy = 0; iy < width / downScale; ++iy)
		for (unsigned int ix = 0; ix < width / downScale; ++ix)
		{
			ids->InsertNextValue( downScale + iy*downScale + (downScale / 2 + ix*downScale)*width);
		}

	auto selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
	selectionNode->SetFieldType(vtkSelectionNode::POINT);
	selectionNode->SetContentType(vtkSelectionNode::INDICES);
	selectionNode->SetSelectionList(ids);

	auto selection = vtkSmartPointer<vtkSelection>::New();
	selection->AddNode(selectionNode);



	extract = vtkSmartPointer<vtkExtractSelectedIds>::New();
	extract->SetInputConnection(0, attribs->GetOutputPort());

	//extract->PreserveTopologyOn();

	extract->SetInputData(1, selection);

	extract->Update();




	auto geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New();

	geometryFilter->AddInputConnection(extract->GetOutputPort());

	geometryFilter->Update();

	glypher = vtkSmartPointer<vtkIconGlyphFilter>::New();
	glypher->UseIconSizeOff();
	glypher->SetDisplaySize(2*downScale, 2 * downScale);

	glypher->SetIconSize(24, 24);
	glypher->SetIconSheetSize(72,24);

	glypher->SetGravityToCenterCenter();

	glypher->AddInputConnection(geometryFilter->GetOutputPort());

	glypher->Update();


	 auto mapReader = vtkSmartPointer<vtkPNGReader>::New();
	 mapReader->SetFileName("icons.png");

	 mapReader->Update();

	 auto t = vtkSmartPointer<vtkTexture>::New();
	 t->SetInputData(mapReader->GetOutput());
	 t->SetInterpolate(false);
	 t->Update();

	 filters.push_back(extract);
	 filters.push_back(geometryFilter);
	 filters.push_back(attribs);
	 filters.push_back(glypher);

	polyMap = vtkSmartPointer<vtkPolyDataMapper>::New();
	polyMap->AddInputConnection(glypher->GetOutputPort());

	SetActiveMapper(polyMap);

	polyAct = vtkSmartPointer<vtkActor>::New();

	polyAct->SetTexture(t);

	polyAct->SetMapper(polyMap);

	SetProp(polyAct);

}



