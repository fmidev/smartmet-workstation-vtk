#include "TimeAnimator.h"

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