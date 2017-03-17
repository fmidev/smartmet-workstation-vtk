#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkSphere.h>
#include <vtkSampleFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolumeProperty.h>
#include <vtkCamera.h>
#include <vtkImageShiftScale.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkXMLImageDataReader.h>

#include <vtkSmartVolumeMapper.h>

#include <vtkPlanes.h>
#include <vtkTransform.h>
#include <vtkCommand.h>
#include <vtkBoxWidget.h>


#include <vtkInteractorStyleTrackball.h>
#include <vtkCubeAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>

#include <vtkPerlinNoise.h>


static void CreateImageData(vtkImageData* im);



class vtkBoxWidgetCallback : public vtkCommand
{
public:
	static vtkBoxWidgetCallback *New()
	{
		return new vtkBoxWidgetCallback;
	}
	void Execute(vtkObject *caller, unsigned long, void*) VTK_OVERRIDE
	{
		vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
		if (this->Mapper)
		{
			vtkPlanes *planes = vtkPlanes::New();
			widget->GetPlanes(planes);
			this->Mapper->SetClippingPlanes(planes);
			planes->Delete();
		}
	}
	void SetMapper(vtkSmartVolumeMapper* m)
	{
		this->Mapper = m;
	}

protected:
	vtkBoxWidgetCallback()
	{
		this->Mapper = 0;
	}

	vtkSmartVolumeMapper *Mapper;
};



int main(int argc, char *argv[])
{
	vtkSmartPointer<vtkImageData> imageData =
		vtkSmartPointer<vtkImageData>::New();
	if (argc < 2)
	{
		CreateImageData(imageData);
	}
	else
	{
		vtkSmartPointer<vtkXMLImageDataReader> reader =
			vtkSmartPointer<vtkXMLImageDataReader>::New();
		reader->SetFileName(argv[1]);
		reader->Update();
		imageData->ShallowCopy(reader->GetOutput());
	}

	vtkSmartPointer<vtkRenderWindow> renWin =
		vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderer> ren1 =
		vtkSmartPointer<vtkRenderer>::New();
	ren1->SetBackground(0.1, 0.4, 0.2);

	renWin->AddRenderer(ren1);

	renWin->SetSize(301, 300); // intentional odd and NPOT  width/height


	vtkSmartPointer<vtkRenderWindowInteractor> iren =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	renWin->Render(); // make sure we have an OpenGL context.


	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper =
		vtkSmartPointer<vtkSmartVolumeMapper>::New();
	volumeMapper->SetBlendModeToComposite(); // composite first
#if VTK_MAJOR_VERSION <= 5
	volumeMapper->SetInputConnection(imageData->GetProducerPort());
#else
	volumeMapper->SetInputData(imageData);
#endif  
	vtkSmartPointer<vtkVolumeProperty> volumeProperty =
		vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->ShadeOff();
	volumeProperty->SetInterpolationType(VTK_NEAREST_INTERPOLATION);

	volumeMapper->SetRequestedRenderModeToGPU();

	vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity =
		vtkSmartPointer<vtkPiecewiseFunction>::New();
	compositeOpacity->AddPoint(0.0, 0.1);
	compositeOpacity->AddPoint(127.0, 0.3);
	compositeOpacity->AddPoint(255.0, 0.8);
	volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.

	vtkSmartPointer<vtkColorTransferFunction> color =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	color->AddRGBPoint(0.0, 0.2, 0.0, 0.0);
	color->AddRGBPoint(127.0, 0, 1.0, 0.0);
	color->AddRGBPoint(255.0, 1.0, 0.0, 0.0);
	volumeProperty->SetColor(color);

	vtkSmartPointer<vtkVolume> volume =
		vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);

	ren1->AddViewProp(volume);

	renWin->Render();

	vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor =
		vtkSmartPointer<vtkCubeAxesActor>::New();
	cubeAxesActor->SetBounds(imageData->GetBounds());
	cubeAxesActor->SetCamera(ren1->GetActiveCamera());

	cubeAxesActor->GetTitleTextProperty(0)->SetColor(1.0, 0.0, 0.0);
	cubeAxesActor->GetLabelTextProperty(0)->SetColor(1.0, 0.0, 0.0);

	cubeAxesActor->GetTitleTextProperty(1)->SetColor(0.0, 1.0, 0.0);
	cubeAxesActor->GetLabelTextProperty(1)->SetColor(0.0, 1.0, 0.0);

	cubeAxesActor->GetTitleTextProperty(2)->SetColor(0.0, 0.0, 1.0);
	cubeAxesActor->GetLabelTextProperty(2)->SetColor(0.0, 0.0, 1.0);

	cubeAxesActor->SetDragable(false);
	cubeAxesActor->SetPickable(false);

	ren1->AddActor(cubeAxesActor);

	renWin->Render();


	vtkBoxWidget *boxWidget = vtkBoxWidget::New();
	boxWidget->SetInteractor(iren);
	boxWidget->SetPlaceFactor(1.1);


	boxWidget->SetTranslationEnabled(true);
	boxWidget->SetRotationEnabled(false);

	boxWidget->SetProp3D(volume);
	boxWidget->PlaceWidget();

	boxWidget->InsideOutOn();

	vtkBoxWidgetCallback *callback = vtkBoxWidgetCallback::New();
	callback->SetMapper( volumeMapper );
	boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);
	callback->Delete();
	boxWidget->GetSelectedFaceProperty()->SetOpacity(0.0);

	renWin->Render();

	ren1->ResetCamera();

	iren->Initialize();
	iren->Start();
	

	return EXIT_SUCCESS;
}

void CreateImageData(vtkImageData* imageData)
{

	double scale = 1.0;

	// Create a spherical implicit function.
	vtkSmartPointer<vtkPerlinNoise> perlin = vtkSmartPointer<vtkPerlinNoise>::New();
	perlin->SetFrequency(0.6, 0.9, 1.2);
	perlin->SetPhase(0, 0, 0);

	vtkSmartPointer<vtkSampleFunction> sampleFunction =
		vtkSmartPointer<vtkSampleFunction>::New();
	sampleFunction->SetImplicitFunction(perlin);
	sampleFunction->SetOutputScalarTypeToDouble();
	sampleFunction->SetSampleDimensions(256, 256, 256);
	sampleFunction->SetModelBounds(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);
	sampleFunction->SetCapping(false);
	sampleFunction->SetComputeNormals(false);
	sampleFunction->SetScalarArrayName("values");
	sampleFunction->Update();

	vtkDataArray* a = sampleFunction->GetOutput()->GetPointData()->GetScalars("values");
	double range[2];
	a->GetRange(range);

	vtkSmartPointer<vtkImageShiftScale> t =
		vtkSmartPointer<vtkImageShiftScale>::New();
	t->SetInputConnection(sampleFunction->GetOutputPort());

	t->SetShift(-range[0]);
	double magnitude = range[1] - range[0];
	if (magnitude == 0.0)
	{
		magnitude = 1.0;
	}
	t->SetScale(255.0 / magnitude);
	t->SetOutputScalarTypeToUnsignedChar();

	t->Update();

	imageData->ShallowCopy(t->GetOutput());
}