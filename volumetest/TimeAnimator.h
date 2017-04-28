#ifndef TIMEANIMATOR_H
#define TIMEANIMATOR_H

#include <vtkSliderRepresentation2D.h>
#include <vtkRenderWindow.h>
#include <vtkCommand.h>
#include <vtkSliderWidget.h>

#include "VisualizerManager.h"
#include "newBaseSourcerMetaData.h"



class TimeAnimator;

class vtkSliderWidgetCallback : public vtkCommand
{
public:
	vtkSliderWidgetCallback() {}

	static vtkSliderWidgetCallback *New()
	{
		return new vtkSliderWidgetCallback;
	}
	void Execute(vtkObject *caller, unsigned long, void*) VTK_OVERRIDE;

	inline void setAnim(TimeAnimator *ta) {
		timeAnim = ta;
	}
protected:
	TimeAnimator *timeAnim;

};


class TimeAnimCallback : public vtkCommand {
public:
	static TimeAnimCallback* New() {
		return new TimeAnimCallback();
	}

	virtual void Execute(vtkObject* caller, unsigned long eventID, void *data);

	inline void setAnim(TimeAnimator *ta) {
		timeAnim = ta;
	}
protected:
	TimeAnimator *timeAnim;

	TimeAnimCallback() {
		timeAnim = nullptr;

	}
};


class TimeAnimator {

	vtkRenderWindow *renWin;
	vtkSliderRepresentation2D * sliderRep;
	vtkSliderWidget *slider;
	VisualizerManager *vm;
	metaData *meta;
	int wrapCount;
	TimeAnimCallback *timerCallback;
	vtkSliderWidgetCallback *sliderCallback;

	double animDelay;

	double timeVal;

	unsigned long timerID;//only valid if enabled is true
	unsigned long timerCallbackTag; 
	unsigned long sliderCallbackTag;
	bool enabled;

public:
	TimeAnimator(vtkRenderWindow *renderWin, vtkSliderWidget *slider, VisualizerManager *visMan, metaData *metadata, double delay = 200) :
		renWin(renderWin), slider(slider), sliderRep(vtkSliderRepresentation2D::SafeDownCast( slider->GetSliderRepresentation() ) ), vm(visMan),
		meta(metadata), wrapCount(0), timerCallbackTag(0), enabled(false),
		timerCallback(TimeAnimCallback::New()), sliderCallback(vtkSliderWidgetCallback::New()), animDelay(delay)
	{
		timeVal = sliderRep->GetValue();
		timerCallback->setAnim(this);
		sliderCallback->setAnim(this);
		timerCallbackTag = renWin->GetInteractor()->AddObserver(vtkCommand::TimerEvent, timerCallback);
		sliderCallbackTag = slider->AddObserver(vtkCommand::InteractionEvent, sliderCallback);
	}
	~TimeAnimator() {
		renWin->GetInteractor()->RemoveObserver(timerCallbackTag);
		slider->RemoveObserver(sliderCallbackTag);
		timerCallback->Delete();
		sliderCallback->Delete();
	}

	void TimeStep(double val) {
		if (val < sliderRep->GetMinimumValue()) val = sliderRep->GetMinimumValue();
		if (val > sliderRep->GetMaximumValue()) val = sliderRep->GetMaximumValue();
		val = floor(val);
		timeVal = val;

		time_t time = meta->timeStepToEpoch(val);
		sliderRep->SetTitleText(metaData::getTimeString(time).c_str());

		sliderRep->SetValue(val);
		vm->UpdateTimeStep(val);

	}
	inline void AnimateStep(bool wrap = true, char dir = 1) {

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


	inline bool IsEnabled() {
		return enabled;
	}

	inline double GetTime() {
		return timeVal;
	}

	inline void StartAnim() {

		renWin->AddObserver(vtkCommand::TimerEvent, timerCallback);
		timerID = renWin->GetInteractor()->CreateRepeatingTimer(animDelay);


		enabled = true;
	}

	inline void StopAnim() {
		renWin->GetInteractor()->DestroyTimer(timerID);
		enabled = false;
	}

	inline void RefreshTimer() {
		if (enabled) {
			StopAnim();
			StartAnim();
		}
		else {
			StartAnim();
			StopAnim();
		}
	}


	inline void ToggleAnim() {
		if (enabled) StopAnim();
		else StartAnim();
	}

	inline void IncreaseDelay() {
		double step = std::max(10.0, animDelay*0.5);
		animDelay += step;
		if (animDelay > 2000) animDelay = 2000;
		RefreshTimer();
	}

	inline void DecreaseDelay() {
		double step = std::max(10.0, animDelay*0.5);
		animDelay -= step;
		if (animDelay < 10) animDelay = 10;
		RefreshTimer();
	}

};



#endif /* TIMEANIMATOR_H */