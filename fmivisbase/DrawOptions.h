#ifndef DrawOptions_h__
#define DrawOptions_h__

#include <memory>
#include <array>
#include <vector>

#include "vtkColor.h"
#include "boost\smart_ptr\shared_ptr.hpp"
#include "smarttools\NFmiDrawParam.h"
#include <NFmiDrawParamFactory.h>
#include "NFmiColor.h"

#include <vtkColorTransferFunction.h>
#include <vtkScalarsToColors.h>
#include <vtkLookupTable.h>
#include <vtkSmartPointer.h>
#include <vtkImageMapper3D.h>


namespace fmiVis {

struct DrawOptions {

	float isolineZero;
	float isolineStep;

	bool isolineSingleCol;
	vtkColor4f isolineCol[3];
	float isolineColVal[3];

	bool labelBox;
	float labelHeight;
	vtkColor4f labelBoxCol;
	int labelDigits;


	int contourStep;
	vtkColor4f contourCol[4];
	float contourVal[4];

	//unused
	int isolineStipple;
	float isolineWidth;
	bool isolineFeather;

};




inline vtkColor4f ColorIndexToRgb(int colorIndex)
{
	constexpr int colorSize = 8;

	if (colorIndex < 2)
		return vtkColor4f(0, 0, 0, 1.0f); // kukaan ei tiedä mitä pitäisi palauttaa
	else if (colorIndex == 2)
		return vtkColor4f(0, 0, 0, 0);

	colorIndex -= 3;
	int blue = colorIndex % colorSize;
	colorIndex = colorIndex / colorSize;
	int green = colorIndex % colorSize;
	colorIndex = colorIndex / colorSize;
	int red = colorIndex % colorSize;

	vtkColor4f rgb;
	rgb[0] = static_cast<float>(round(255. / (colorSize - 1) * red));
	rgb[1] = static_cast<float>(round(255. / (colorSize - 1) * green));
	rgb[2] = static_cast<float>(round(255. / (colorSize - 1) * blue));
	rgb[4] = 1.0f;

	return rgb;
}



std::shared_ptr<NFmiDrawParamFactory> LoadOptions();


inline vtkColor4f fmiToVtkColor(NFmiColor col) {
	return vtkColor4f(col.Red(), col.Green(), col.Blue(),col.Alpha());
		
}


inline vtkSmartPointer<vtkScalarsToColors> constColor(vtkColor4f *c)
{
	auto f = vtkSmartPointer<vtkLookupTable>::New();
	f->SetNumberOfTableValues(1);
	f->SetTableValue(0,c->GetRed(),c->GetGreen(),c->GetBlue());
	return f;
}

vtkSmartPointer<vtkScalarsToColors> blueToRedColFunc(double min, double max);
vtkSmartPointer<vtkScalarsToColors> redToGreenColFunc(double min, double max);

vtkSmartPointer<vtkScalarsToColors> makeIsolineColorFunction(const NFmiDrawParam *drawParam);
vtkSmartPointer<vtkScalarsToColors> makeContourColorFunction(const NFmiDrawParam *drawParam);

vtkSmartPointer<vtkScalarsToColors> makeCustomIsolineColorFunction(const NFmiDrawParam *drawParam);
vtkSmartPointer<vtkScalarsToColors> makeCustomContourColorFunction(const NFmiDrawParam *drawParam);

void readIsolineParams(const NFmiDrawParam *drawParam, std::array<vtkColor4f, 3> &isolineCol, std::array<float, 3> &isolineColVal);

void readContourParams(const NFmiDrawParam *drawParam, std::array<vtkColor4f, 4> &contourCol, std::array<float, 4> &contourColVal);

void readCustomIsolineParams(const NFmiDrawParam *drawParam, std::vector<vtkColor4f> &isolineCol, std::vector<float> &isolineColVal);

void readCustomContourParams(const NFmiDrawParam *drawParam, std::vector<vtkColor4f> &contourCol, std::vector<float> &contourColVal);



template <size_t n>
vtkSmartPointer<vtkScalarsToColors> makeColorFunction(std::array<vtkColor4f, n> &isolineCol, std::array<float, n> &isolineColVal)
{
	auto f = vtkSmartPointer<vtkColorTransferFunction>::New();
	for (size_t i = 0; i < n; i++)
		f->AddRGBPoint(isolineColVal[i],
			isolineCol[i].GetRed(), isolineCol[i].GetGreen(), isolineCol[i].GetBlue());
	f->SetClamping(true);
	return f;
}

inline vtkSmartPointer<vtkScalarsToColors> makeColorFunction(std::vector<vtkColor4f> &isolineCol, std::vector<float> &isolineColVal) {

	auto f = vtkSmartPointer<vtkColorTransferFunction>::New();

	for (size_t i = 0; i < isolineColVal.size(); i++)
		f->AddRGBPoint(isolineColVal[i],
			isolineCol[i].GetRed(), isolineCol[i].GetGreen(), isolineCol[i].GetBlue());
	f->SetClamping(true);
	return f;
}

}

#endif // DrawOptions_h__
