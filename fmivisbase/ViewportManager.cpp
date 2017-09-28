#include "ViewportManager.h"

#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include "VisualizerManager.h"


void fmiVis::ViewportManager::UpdateTimeStep(double t)
{
	for (auto &vp : viewports) {
		vp.vm->UpdateTimeStep(t);
	}
}

void fmiVis::ViewportManager::Update()
{
	for (auto &vp : viewports) {
		vp.vm->Update();
	}
}

size_t fmiVis::ViewportManager::whichViewport(vtkRenderer *vp)
{
	for (size_t i = 0; i < viewports.size(); i++) {
		if (vp == viewports[i].r.Get() ) return i;
	}
	return -1;
}

void fmiVis::ViewportManager::ViewportNumKey(ViewportID vpid, int vid, bool ctrl)
{
	if (vpid < 0 || vpid >= viewports.size())
		throw std::invalid_argument("invalid vpid");

	auto &vm = *viewports[vpid].vm;
	auto &visualizerTexts = viewports[vpid].vt;

	if (vid < 0 || vid >= vm.GetVisNum())
		throw std::invalid_argument("invalid vid");

	if (ctrl) {

		vm.ToggleMode(vid);
		visualizerTexts->at(vid)->GetTextProperty()->SetItalic(!visualizerTexts->at(vid)->GetTextProperty()->GetItalic());
	}
	else {

		if (vm.IsEnabled(vid)) {
			visualizerTexts->at(vid)->GetTextProperty()->SetColor(0.2, 0.2, 0.2);
		}
		else {
			visualizerTexts->at(vid)->GetTextProperty()->SetColor(0.8, 0.8, 0.8);
		}

		vm.ToggleVis(vid);
	}

}

void fmiVis::ViewportManager::AddViewport(vtkSmartPointer<vtkRenderer> r, std::unique_ptr<VisualizerManager> &&vm, std::unique_ptr<std::vector<vtkSmartPointer<vtkTextActor>>> &&vt)
{
	viewports.emplace_back(r, std::move(vm), std::move(vt));
}
