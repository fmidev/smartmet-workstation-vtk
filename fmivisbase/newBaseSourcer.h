#ifndef NEWBASESOURCER_H
#define NEWBASESOURCER_H

#include <memory>
#include <functional>
#include <vector>

#include <vtkImageData.h>
#include <vtkImageAlgorithm.h>
#include "newBaseSourcerBase.h"

class NFmiQueryData;
class NFmiFastQueryInfo;
class NFmiDataIdent;

namespace fmiVis {

	struct nbsMetadata;

	//This class interfaces with the low level newbase functions to read a sqd file and feeds it into the VTK pipeline
	//it also performs basic interpolation from the arbitrary grid of sql to the regular grids of imagedata, with some bolted-on threading no less
	class newBaseSourcer : public newBaseSourcerBase, public vtkImageAlgorithm {

	protected:
		vtkImageData* im;
		//	float* heights;
		std::vector<float> heights;


		int subSample;

		size_t dimension;


		virtual void ResetImage(bool realloc = false);

		void InterpolateImage(int startX, int startY, int endX, int endY);
		void AllocateHeights();

		int getHeight(int x, int y, int z);

		void ReadHeights(int time);

		inline void freeRes() {
			if (im)
				im->Delete();
			// 		if (heights)
			// 			delete[] heights;
		}


	public:

		newBaseSourcer(const std::string &file, nbsMetadata *meta, int param, int res = 80, int subSample = 1, size_t dimension = 1);

		void Delete() override {
			freeRes();
		}


		//method for querying the available data
		int newBaseSourcer::RequestInformation(vtkInformation* vtkNotUsed(request),
			vtkInformationVector** vtkNotUsed(inputVector),
			vtkInformationVector* outputVector);

		//actual request for data
		virtual int newBaseSourcer::RequestData(vtkInformation* vtkNotUsed(request),
			vtkInformationVector** vtkNotUsed(inputVector),
			vtkInformationVector* outputVector);

		void setSubSample(int s) {
			subSample = s;
			ResetImage(true);
			AllocateHeights();
			prevTime = -1;
		}

		inline int getParam() {
			return param;
		}

		const NFmiDataIdent getParamDescr();

	protected:
		virtual newBaseSourcer::~newBaseSourcer();
		newBaseSourcer(const newBaseSourcer &copy) = delete;
		void operator=(const newBaseSourcer &assign) = delete;


		//interpolation helpers
		void inline SetValue(float *a, float *x) {
			for (int i = 0; i < dimension; i++)
				a[i] = x[i];
		}
		void inline SetValue(float *a, std::vector<float> &x) {
			SetValue(a, &x[0]);
		}
		void inline SetValue(std::vector<float> &a, float *x) {
			SetValue(&a[0], x);
		}
		void inline SetValue(std::vector<float> &a, std::vector<float> &x) {
			SetValue(&a[0], &x[0]);
		}

		bool ValidValue(float *val);
		bool ValidValue(std::vector<float> &val);


	};

};
#endif /*NEWBASESOURCER_H*/