#include "DrawOptions.h"

#include <algorithm>

#include "vtkSmartPointer.h"
#include <vtkPiecewiseFunction.h>



std::shared_ptr<NFmiDrawParamFactory> fmiVis::LoadOptions(std::string file)
{
	std::shared_ptr<NFmiDrawParamFactory> drawParamFactory = std::make_shared<NFmiDrawParamFactory>(false, false);
	drawParamFactory->LoadDirectory(file);
	drawParamFactory->Init();

	return drawParamFactory;
}

vtkSmartPointer<vtkScalarsToColors> fmiVis::blueToRedColFunc(double min, double max)
{
	static std::array<vtkColor4f, 2> col;
	static std::array<float, 2> val;


	col[0] = { 0 , 0 , 1.0f, 1.0f };
	col[1] = { 1.0f, 0 , 0 , 1.0f };

	val[0] = min;
	val[1] = max;

	return makeColorFunction(col, val);
}

vtkSmartPointer<vtkScalarsToColors> fmiVis::redToGreenColFunc(double min, double max)
{
	static std::array<vtkColor4f, 2> col;
	static std::array<float, 2> val;

	col[0] = { 1.0f, 0 , 0 , 1.0f };
	col[1] = { 0 , 1.0f, 0 , 1.0f };

	val[0] = min;
	val[1] = max;

	return makeColorFunction(col, val);
}

vtkSmartPointer<vtkPiecewiseFunction> fmiVis::opacityFunction(double min, double max, double threshold, double maxAlpha/*=0.2*/, double minAlpha/*=0.0*/)
{
	auto f = vtkSmartPointer<vtkPiecewiseFunction>::New();
	f->AddPoint(min, 0.0f);
	f->AddPoint(threshold, minAlpha);
	f->AddPoint(max, maxAlpha);
	return f;
}

vtkSmartPointer<vtkScalarsToColors> fmiVis::makeIsolineColorFunction(const NFmiDrawParam *drawParam)
{
	std::array<vtkColor4f, 3> isolineCol;
	std::array<float, 3> isolineColVal;
	readIsolineParams(drawParam, isolineCol, isolineColVal);
	return makeColorFunction(isolineCol, isolineColVal);
}

vtkSmartPointer<vtkScalarsToColors> fmiVis::makeContourColorFunction(const NFmiDrawParam *drawParam)
{
	std::array<vtkColor4f, 4> contourCol;
	std::array<float, 4> contourColVal;
	readContourParams(drawParam, contourCol, contourColVal);
	return makeColorFunction(contourCol, contourColVal);
}
/*
vtkSmartPointer<vtkScalarsToColors> fmiVis::makeCustomIsolineColorFunction(const NFmiDrawParam *drawParam)
{
	std::vector<vtkColor4f> contourCol;
	std::vector<float> contourColVal;
	readCustomContourParams(drawParam, contourCol, contourColVal);
	return makeColorFunction(contourCol, contourColVal);
}




vtkSmartPointer<vtkScalarsToColors> fmiVis::makeCustomContourColorFunction(const NFmiDrawParam *drawParam)
{
	std::vector<vtkColor4f> isolineCol;
	std::vector<float> isolineColVal;
	readCustomIsolineParams(drawParam, isolineCol, isolineColVal);
	return makeColorFunction(isolineCol, isolineColVal);
}*/


void fmiVis::readIsolineParams(const NFmiDrawParam *drawParam, std::array<vtkColor4f, 3> &isolineCol, std::array<float, 3> &isolineColVal)
{

	isolineColVal[0] = drawParam->SimpleIsoLineColorShadeLowValue();
	isolineColVal[1] = drawParam->SimpleIsoLineColorShadeMidValue();
	isolineColVal[2] = drawParam->SimpleIsoLineColorShadeHighValue();

	isolineCol[0] = fmiToVtkColor(drawParam->SimpleIsoLineColorShadeLowValueColor());
	isolineCol[1] = fmiToVtkColor(drawParam->SimpleIsoLineColorShadeMidValueColor());
	isolineCol[2] = fmiToVtkColor(drawParam->SimpleIsoLineColorShadeHighValueColor());
}

void fmiVis::readContourParams(const NFmiDrawParam *drawParam, std::array<vtkColor4f, 4> &contourCol, std::array<float, 4> &contourColVal)
{
	contourCol[0] = fmiToVtkColor(drawParam->ColorContouringColorShadeLowValueColor());
	contourCol[1] = fmiToVtkColor(drawParam->ColorContouringColorShadeMidValueColor());
	contourCol[2] = fmiToVtkColor(drawParam->ColorContouringColorShadeHighValueColor());
	contourCol[3] = fmiToVtkColor(drawParam->ColorContouringColorShadeHigh2ValueColor());

	contourColVal[0] = drawParam->ColorContouringColorShadeLowValue();
	contourColVal[1] = drawParam->ColorContouringColorShadeMidValue();
	contourColVal[2] = drawParam->ColorContouringColorShadeHighValue();
	contourColVal[3] = drawParam->ColorContouringColorShadeHigh2Value();
}

void fmiVis::readCustomIsolineParams(const NFmiDrawParam *drawParam, std::vector<vtkColor4f> &isolineCol, std::vector<float> &isolineColVal) {

	auto colIter = begin( drawParam->SpecialIsoLineColorIndexies() );

	isolineCol.resize( drawParam->SpecialIsoLineColorIndexies().size() );

	std::generate(begin(isolineCol), end(isolineCol), [&colIter]() {
		return ColorIndexToRgb(*colIter++);
	});


	auto valIter = begin( drawParam->SpecialIsoLineValues() );

	isolineColVal.resize( drawParam->SpecialIsoLineValues().size() );

	std::generate(begin(isolineColVal), end(isolineColVal), [&valIter]() {
	return *valIter++;
	});
}

void fmiVis::readCustomContourParams(const NFmiDrawParam *drawParam, std::vector<vtkColor4f> &contourCol, std::vector<float> &contourColVal) {

	auto colIter = begin(drawParam->SpecialContourColorIndexies());

	contourCol.resize(drawParam->SpecialContourColorIndexies().size());

	std::generate(begin(contourCol), end(contourCol), [&colIter]() {
		return ColorIndexToRgb(*colIter++);
	});


	auto valIter = begin( drawParam->SpecialContourValues() );

	contourColVal.resize ( drawParam->SpecialContourValues().size() );

	std::generate(begin(contourColVal), end(contourColVal), [&valIter]() {
		return *valIter++;
	});

}