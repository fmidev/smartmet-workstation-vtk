#ifndef DrawOptions_h__
#define DrawOptions_h__

#include <memory>
#include <array>
#include <vector>
#include <functional>

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
#include <vtkPiecewiseFunction.h>


#include <json_spirit.h>

#include <unordered_map>

#include "vtkSmartVolumeMapper.h"



using namespace std::string_literals;

//various functions to load smartmet drawParam options into 2D visualizers, and some groundwork for a json-based 3D options format

namespace fmiVis {
	typedef std::array<float, 4> arrayCol;
	
	struct colorDef {
		std::vector<arrayCol> col;
		std::vector<float> val;
	};


	inline vtkColor4f ColorIndexToRgb(int colorIndex)
	{
		constexpr int colorSize = 8;

		if (colorIndex < 2)
			return vtkColor4f(0, 0, 0, 1.0f); //unspecified
		else if (colorIndex == 2)
			return vtkColor4f(0, 0, 0, 0);

		colorIndex -= 3;
		int blue = colorIndex % colorSize;
		colorIndex = colorIndex / colorSize;
		int green = colorIndex % colorSize;
		colorIndex = colorIndex / colorSize;
		int red = colorIndex % colorSize;

		vtkColor4f rgb;
		rgb[0] = static_cast<float>(red) / colorSize;
		rgb[1] = static_cast<float>(green) / colorSize;
		rgb[2] = static_cast<float>(blue) / colorSize;
		rgb[3] = 1.0f;

		return rgb;
	}



	std::shared_ptr<NFmiDrawParamFactory> LoadOptions(std::string file);


	inline vtkColor4f fmiToVtkColor(NFmiColor col) {
		return vtkColor4f(col.Red(), col.Green(), col.Blue(), col.Alpha());

	}


	inline vtkSmartPointer<vtkScalarsToColors> constColor(vtkColor4f *c)
	{
		auto f = vtkSmartPointer<vtkLookupTable>::New();
		f->SetNumberOfTableValues(1);
		f->SetTableValue(0, c->GetRed(), c->GetGreen(), c->GetBlue());
		return f;
	}

	vtkSmartPointer<vtkScalarsToColors> blueToRedColFunc(double min, double max);
	vtkSmartPointer<vtkScalarsToColors> redToGreenColFunc(double min, double max);
	vtkSmartPointer<vtkPiecewiseFunction> opacityFunction(float min, float max, float threshold, float maxAlpha = 0.2, float minAlpha = 0.0);

	vtkSmartPointer<vtkPiecewiseFunction> makeOpacityFunction(const std::vector<float> &val, const std::vector<float> &alpha);

	vtkSmartPointer<vtkScalarsToColors> makeIsolineColorFunction(const NFmiDrawParam *drawParam);
	vtkSmartPointer<vtkScalarsToColors> makeContourColorFunction(const NFmiDrawParam *drawParam);

	vtkSmartPointer<vtkScalarsToColors> makeCustomIsolineColorFunction(const NFmiDrawParam *drawParam);
	vtkSmartPointer<vtkScalarsToColors> makeCustomContourColorFunction(const NFmiDrawParam *drawParam);

	void readIsolineParams(const NFmiDrawParam *drawParam, std::array<vtkColor4f, 3> &isolineCol, std::array<float, 3> &isolineColVal);

	void readContourParams(const NFmiDrawParam *drawParam, std::array<vtkColor4f, 4> &contourCol, std::array<float, 4> &contourColVal);

	void readCustomIsolineParams(const NFmiDrawParam *drawParam, std::vector<vtkColor4f> &isolineCol, std::vector<float> &isolineColVal);

	void readCustomContourParams(const NFmiDrawParam *drawParam, std::vector<vtkColor4f> &contourCol, std::vector<float> &contourColVal);



	vtkSmartPointer<vtkScalarsToColors> jsonTransferFunction(const  json_spirit::mObject &obj);

	vtkSmartPointer< vtkPiecewiseFunction> jsonOpacityFunction(const json_spirit::mObject &obj);



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

	inline vtkSmartPointer<vtkScalarsToColors> makeColorFunction(std::vector<vtkColor4f> &col, std::vector<float> &val) {

		auto f = vtkSmartPointer<vtkColorTransferFunction>::New();


		for (size_t i = 0; i < val.size(); i++) {
			int clamped = i;
			if (i >= col.size()) clamped = col.size() - 1;
			f->AddRGBPoint(val[i],
				col[clamped].GetRed(), col[clamped].GetGreen(), col[clamped].GetBlue());
		}
		f->SetClamping(true);
		return f;
	}

	
	//a generic json enum - a fixed set of strings in the json object associated with a function to apply the option

	typedef std::pair<std::string, int> enumKey;

	template <typename T>
	struct jsonEnum {
		const std::unordered_map<std::string, int> members;
		virtual void apply(const std::string&, T*) = 0;

		jsonEnum(const std::unordered_map<std::string, int> m) : members(m) {};
	};


	struct jsonEnumInterpolation : jsonEnum<vtkSmartVolumeMapper> {
		jsonEnumInterpolation() : jsonEnum(
			{ enumKey{"NEAREST"s,VTK_NEAREST_INTERPOLATION},
			enumKey{"LINEAR"s,VTK_LINEAR_INTERPOLATION},
			enumKey{"CUBIC"s,VTK_CUBIC_INTERPOLATION} })
		{}

		void apply(const std::string& interp, vtkSmartVolumeMapper *mapper) override {
			mapper->SetInterpolationMode(members.at(interp));
		}
	};


	struct DrawOptions {
		int version;

		bool shading;

		std::string interpolation;

		colorDef volColor;
		colorDef conColor;

		float contourThreshold;

		bool useScalarTree;

		DrawOptions(const json_spirit::mObject &obj);

	};

	json_spirit::mObject serializeOptions(const DrawOptions &opt);

}
#endif // DrawOptions_h__
