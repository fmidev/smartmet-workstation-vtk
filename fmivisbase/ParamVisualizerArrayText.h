#ifndef ParamVisualizerArrayText_h__
#define ParamVisualizerArrayText_h__

#include <vtkSmartPointer.h>


#include "ParamVisualizerBase.h"

#include <vtkSmartPointer.h>

class vtkTextActor3D;
class vtkPropAssembly;
class vtkColorTransferFunction;

class ParamVisualizerArrayText : public ParamVisualizerBase {

	std::vector<vtkTextActor3D*> acts;

	vtkSmartPointer<vtkPropAssembly> assembly;

	vtkSmartPointer<vtkColorTransferFunction> colF;

public:
	ParamVisualizerArrayText(const std::string &file, nbsMetadata &m, int param, vtkSmartPointer<vtkColorTransferFunction> col = nullptr);
	~ParamVisualizerArrayText();

	void UpdateTimeStep(double t) override;

};

#endif // ParamVisualizerArrayText_h__
