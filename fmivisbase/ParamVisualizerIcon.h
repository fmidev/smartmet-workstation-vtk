#ifndef ParamVisualizerIcon_h__
#define ParamVisualizerIcon_h__

#include <memory>
#include <functional>

#include <vtkSmartPointer.h>

#include "ParamVisualizerBase.h"

class vtkIconGlyphFilter;

class vtkPolyDataMapper;

class vtkContourFilter;
class vtkColorTransferFunction;
class vtkActor;
class vtkPolyData;
class vtkExtractSelectedIds;
class vtkProgrammableAttributeDataFilter;
class NFmiDrawParamFactory;
class NFmiDataIdent;

typedef std::function<float(float)> iconMapping;

class ParamVisualizerIcon : public ParamVisualizerBase {
protected:


	vtkSmartPointer<vtkExtractSelectedIds> extract;

	vtkSmartPointer<vtkProgrammableAttributeDataFilter> attribs;

	vtkSmartPointer<vtkIconGlyphFilter> glypher;

	vtkSmartPointer<vtkPolyDataMapper> polyMap;

	iconMapping mapping;

	vtkSmartPointer<vtkActor> polyAct;

	//false = color
	bool mode;

	friend void AttributeCallback(void* arg);
public:
	ParamVisualizerIcon(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac,iconMapping mapping);

	virtual inline void ToggleMode() { }
};

#endif // ParamVisualizerIcon_h__
