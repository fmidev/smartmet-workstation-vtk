#ifndef ViewportManagerTimegrid_h__
#define ViewportManagerTimegrid_h__


#include "ViewportManager.h"


namespace fmiVis {

	class ViewportManagerTimegrid: public ViewportManager {
		size_t sizeX, sizeY;

		double timeStep;

		vtkSmartPointer<vtkRenderer> baseVP;
	public:

		ViewportManagerTimegrid(size_t sx, size_t sy) : sizeX(sx), sizeY(sy), timeStep(1) {}

		void UpdateTimeStep(double t);

		void ViewportNumKey(ViewportID vpid, int vid, bool ctrl);

		inline void SetBaseViewport(vtkSmartPointer<vtkRenderer> r) 
		{
			baseVP = r;
		}

	};


}


#endif // ViewportManagerTimegrid_h__
