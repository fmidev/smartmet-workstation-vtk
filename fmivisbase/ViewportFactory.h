#ifndef ViewportFactory_h__
#define ViewportFactory_h__

#include <memory>
#include <map>

#include <vtkSmartPointer.h>

class nbsMetadata;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class fmiVisInteractor2D;

namespace fmiVis {


	class ViewportManager;
	class ViewportManagerTimegrid;

	class ViewportFactory {
	public:
		static std::map<int, std::string> paramsSurf;
		static void MakeSingleView(std::string file, nbsMetadata &meta, ViewportManager &viewportMan,
			vtkRenderWindowInteractor *iren, vtkRenderWindow *renWin, fmiVisInteractor2D *style);


		static void MakeTimeGridView(size_t numX, size_t numY,std::string file,nbsMetadata &meta,ViewportManagerTimegrid &viewportMan,
			vtkRenderWindowInteractor *iren, vtkRenderWindow *renWin, fmiVisInteractor2D *style);



	};
}


#endif // ViewportFactory_h__