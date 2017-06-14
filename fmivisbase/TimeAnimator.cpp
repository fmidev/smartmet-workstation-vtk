#include "TimeAnimator.h"

#include <vtkSmartPointer.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSliderWidget.h>

#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include "VisualizerManager.h"
#include "nbsMetadata.h"


void vtkSliderWidgetCallback::Execute(vtkObject *caller, unsigned long, void*)
{
	vtkSliderWidget *sliderWidget =
		reinterpret_cast<vtkSliderWidget*>(caller);
	auto value = static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue();


	timeAnim->TimeStep(value);
}

void TimeAnimCallback::Execute(vtkObject* caller, unsigned long eventID, void *data) {
	timeAnim->AnimateStep();
}

TimeAnimator::TimeAnimator(vtkRenderWindow *renderWin, vtkSliderWidget *slider, VisualizerManager *visMan, nbsMetadata *metadata, double delay /*= 200*/) :
	renWin(renderWin), slider(slider), sliderRep(vtkSliderRepresentation2D::SafeDownCast(slider->GetSliderRepresentation())), vm(visMan),
	meta(metadata), wrapCount(0), timerCallbackTag(0), enabled(false),
	timerCallback(TimeAnimCallback::New()), sliderCallback(vtkSliderWidgetCallback::New()), animDelay(delay)
{
	timeVal = sliderRep->GetValue();
	timerCallback->setAnim(this);
	sliderCallback->setAnim(this);
	timerCallbackTag = renWin->GetInteractor()->AddObserver(vtkCommand::TimerEvent, timerCallback);
	sliderCallbackTag = slider->AddObserver(vtkCommand::InteractionEvent, sliderCallback);


	vtkRenderer *ren = renWin->GetRenderers()->GetFirstRenderer();
	int *winSize = renWin->GetSize();



	timeText = vtkTextActor::New();
	

	timeText->GetTextProperty()->SetFontSize(20);

	timeText->SetDisplayPosition(winSize[0]-380, 35);

	ren->AddActor2D(timeText);


	RefreshTimer();
}

TimeAnimator::~TimeAnimator()
{
	renWin->GetInteractor()->RemoveObserver(timerCallbackTag);
	slider->RemoveObserver(sliderCallbackTag);
	timerCallback->Delete();
	sliderCallback->Delete();
	renWin->GetRenderers()->GetFirstRenderer()->RemoveActor2D(timeText);
	timeText->Delete();
}

void TimeAnimator::TimeStep(double val)
{
	if (val < sliderRep->GetMinimumValue()) val = sliderRep->GetMinimumValue();
	if (val > sliderRep->GetMaximumValue()) val = sliderRep->GetMaximumValue();
	val = floor(val);
	timeVal = val;

	time_t time = meta->timeStepToEpoch(val);
	sliderRep->SetTitleText(nbsMetadata::getTimeString(time).c_str());

	sliderRep->SetValue(val);
	vm->UpdateTimeStep(val);
}

void TimeAnimator::AnimateStep(bool wrap /*= true*/, char dir /*= 1*/)
{
	if (timeVal == meta->timeSteps) {
		wrapCount++;
	}
	else wrapCount = 0;

	if (wrapCount > 3 && wrap) {

		wrapCount = 0;
		timeVal = 0;

	}
	else timeVal = floor(timeVal + dir);

	TimeStep(timeVal);

	renWin->Render();
}

void TimeAnimator::StartAnim()
{
	renWin->AddObserver(vtkCommand::TimerEvent, timerCallback);
	timerID = renWin->GetInteractor()->CreateRepeatingTimer(animDelay);

	std::ostringstream s;
	s << "Animation: enabled ( " << animDelay << " ms )";
	timeText->SetInput(s.str().c_str());

	timeText->GetTextProperty()->SetColor(0.8, 0.8, 0.8);


	renWin->Render();


	enabled = true;
}

void TimeAnimator::StopAnim()
{
	renWin->GetInteractor()->DestroyTimer(timerID);

	std::ostringstream s;
	s << "Animation: disabled ( " << animDelay << " ms )";
	timeText->SetInput(s.str().c_str());

	timeText->GetTextProperty()->SetColor(0.2, 0.2, 0.2);

	renWin->Render();

	enabled = false;
}
