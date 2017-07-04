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

typedef std::function<float(float)> iconMapping;

class ParamVisualizerIcon : public ParamVisualizerBase {
protected:


	vtkExtractSelectedIds *extract;

	vtkProgrammableAttributeDataFilter *attribs;

	vtkIconGlyphFilter *glypher;

	vtkPolyDataMapper *polyMap;

	iconMapping mapping;

	vtkActor *polyAct;

	//false = color
	bool mode;

	friend void AttributeCallback(void* arg);
public:
	ParamVisualizerIcon(const std::string &file, nbsMetadata &m, int param, iconMapping mapping);
	~ParamVisualizerIcon();

	virtual inline void ToggleMode() { }
};

#endif // ParamVisualizerIcon_h__
