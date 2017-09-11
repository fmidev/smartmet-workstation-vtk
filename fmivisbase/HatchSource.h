#ifndef HatchSource_h__
#define HatchSource_h__

#include <vtkPolyDataAlgorithm.h>

class vtkPolyData;

class HatchSource : public vtkPolyDataAlgorithm {

	vtkPolyData *data;

	int width;
	int height;

	int density;

	void Generate();

public:

	static HatchSource* New() {
		return new HatchSource();
	}

	int RequestData(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);

	void Generate(int w, int h,int d) {

		width = w;
		height = h;
		density = d;

		Generate();
	}

protected:
	HatchSource();
	~HatchSource();

private:
	void operator=(const HatchSource &assign) = delete;
	HatchSource(const HatchSource &copy) = delete;
};

#endif // HatchSource_h__
