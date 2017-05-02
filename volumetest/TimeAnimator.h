#ifndef TIMEANIMATOR_H
#define TIMEANIMATOR_H

#include <algorithm>

#include <vtkCommand.h>

class vtkRenderWindow;
class vtkSliderRepresentation2D;
class vtkSliderWidget;
class VisualizerManager;
class TimeAnimCallback;
class vtkSliderWidgetCallback;

struct nbsMetadata;

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
	nbsMetadata *meta;
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
	TimeAnimator(vtkRenderWindow *renderWin, vtkSliderWidget *slider, VisualizerManager *visMan, nbsMetadata *metadata, double delay = 200);
	~TimeAnimator();

	void TimeStep(double val);
	void AnimateStep(bool wrap = true, char dir = 1);


	inline bool IsEnabled() {
		return enabled;
	}

	inline double GetTime() {
		return timeVal;
	}

	void StartAnim();

	void StopAnim();

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