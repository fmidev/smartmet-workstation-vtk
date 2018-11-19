#include "DrawOptions.h"

#include <algorithm>
#include <vector>
#include <string>


#include "vtkSmartPointer.h"
#include <vtkPiecewiseFunction.h>




using namespace std::string_literals;

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

vtkSmartPointer<vtkPiecewiseFunction> fmiVis::opacityFunction(float min, float max, float threshold, float maxAlpha/*=0.2*/, float minAlpha/*=0.0*/)
{
	return makeOpacityFunction( { min,threshold,max }, { 0,minAlpha,maxAlpha } );
}

vtkSmartPointer<vtkPiecewiseFunction> fmiVis::makeOpacityFunction(const std::vector<float> &val, const std::vector<float> &alpha) {

	if (val.size() != alpha.size() ) throw std::invalid_argument("Vector size mismatch");
	if(val.size() == 0) throw std::invalid_argument("Empty vector arguments");

	auto f = vtkSmartPointer<vtkPiecewiseFunction>::New();

	auto alphaIter = alpha.begin();

	for (auto &value : val) {
		f->AddPoint(value, *alphaIter++);
	}

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

//lambdas will be easily chainable for handling nested objects

auto valueToFloat = [](const json_spirit::mValue &val) -> float {
	return val.get_real();
};
auto valueToInt = [](const json_spirit::mValue &val) -> int {
	return val.get_int();
};
auto valueToBool = [](const json_spirit::mValue &val) -> bool {
	return val.get_bool();
};

auto valueToArray = [](const json_spirit::mValue &val) -> const json_spirit::mArray& {
	return val.get_array();
};

auto valueToString = [](const json_spirit::mValue &val) -> const std::string& {
	return val.get_str();
};

auto valueToObject = [](const json_spirit::mValue &val) -> const json_spirit::mObject& {
	return val.get_obj();
};

auto jsonArrayToFloat = [](const json_spirit::mArray &arr) -> std::vector<float> {
	std::vector<float> ret;
	ret.reserve(arr.size());

	for (auto i : arr) ret.emplace_back(valueToFloat(i) );

	return ret;
};

auto jsonArrayToColor = [](const json_spirit::mArray &arr) -> fmiVis::arrayCol {
	std::array<float, 4> ret;

	std::transform(begin(arr), end(arr), begin(ret), valueToFloat);

	return ret;
};

auto jsonColorArrayToColors = [](const json_spirit::mArray &arr) -> std::vector< fmiVis::arrayCol > {
	std::vector<fmiVis::arrayCol > ret;
	ret.reserve(arr.size());

	for (auto val : arr) ret.emplace_back( jsonArrayToColor( valueToArray(val) ) );

	return ret;

};

auto colorArrayToVTKColor = [](const fmiVis::arrayCol &arr) -> vtkColor4f {
	vtkColor4f ret;
	ret.SetRed(arr[0]);
	ret.SetGreen(arr[1]);
	ret.SetBlue(arr[2]);
	ret.SetAlpha(arr[3]);

	return ret;
};


auto jsonToColorDef = [](const json_spirit::mObject &obj) {

	auto colorArrays = jsonColorArrayToColors(obj.at("colors"s).get_array());

	std::vector<fmiVis::arrayCol> colors;
	colors.reserve(colorArrays.size());

	for (auto colArray : colorArrays) colors.emplace_back((colArray));

	auto values = jsonArrayToFloat(obj.at("values"s).get_array());

	return 	fmiVis::colorDef{ colors, values };
};

vtkSmartPointer<vtkScalarsToColors> fmiVis::jsonTransferFunction(const json_spirit::mObject &obj)
{
	//auto func = vtkSmartPointer<vtkColorTransferFunction>::New();

	auto colorArrays = jsonColorArrayToColors(obj.at("colors"s).get_array());

	std::vector<vtkColor4f> colors;
	colors.reserve(colorArrays.size());

	for (auto colArray : colorArrays) colors.emplace_back( colorArrayToVTKColor(colArray) );

	auto values = jsonArrayToFloat(obj.at("values"s).get_array());

	return makeColorFunction(colors ,  values );

}

vtkSmartPointer< vtkPiecewiseFunction> fmiVis::jsonOpacityFunction(const json_spirit::mObject &obj) {


	auto jsonValues = obj.at("values"s).get_array();
	auto jsonOpacity = obj.at("opacity"s).get_array();

	std::vector<float> values = jsonArrayToFloat(jsonValues);
	std::vector<float> alpha = jsonArrayToFloat(jsonOpacity);

	return makeOpacityFunction(values, alpha);
}

json_spirit::mObject fmiVis::serializeOptions(const DrawOptions &opt)
{
	json_spirit::mObject ret;

	ret["version"s] = opt.version;

	ret["shading"s] = opt.shading;

	ret["interpolation"s] = opt.interpolation;

	//you need to be careful with the constructor syntax here
	json_spirit::mObject volColor;
	volColor["values"s] = json_spirit::mArray{ opt.volColor.val.begin(), opt.volColor.val.end() };
	volColor["colors"s] = json_spirit::mArray{};

	for (auto col : opt.volColor.col) {
		volColor["colors"s].get_array().push_back( json_spirit::mArray{col.begin(),col.end()} );
	}

	ret["volColor"s] = volColor;


	json_spirit::mObject conColor;
	conColor["values"s] = json_spirit::mArray{ opt.conColor.val.begin(), opt.conColor.val.end() };
	conColor["colors"s] = json_spirit::mArray{};

	for (auto col : opt.conColor.col) {
		conColor["colors"s].get_array().push_back(json_spirit::mArray{ col.begin(),col.end() });
	}

	ret["conColor"s] = conColor;

	ret["contourThreshold"s] = opt.contourThreshold;

	ret["useScalarTree"s] = opt.useScalarTree;

	return ret;


}

fmiVis::DrawOptions::DrawOptions(const json_spirit::mObject &obj)
{
	version = valueToInt(obj.at("version"s));

	shading = valueToBool(obj.at("shading"s));

	interpolation = valueToString(obj.at("interpolation"s));

	volColor = jsonToColorDef(valueToObject(obj.at("volColor")));
	conColor = jsonToColorDef(valueToObject(obj.at("conColor"s)));

	contourThreshold = valueToFloat(obj.at("contourThreshold"s));

	useScalarTree = valueToBool(obj.at("useScalarTree"s));

}
