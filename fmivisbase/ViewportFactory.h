#ifndef ViewportFactory_h__
#define ViewportFactory_h__

#include <memory>

#include <vtkSmartPointer.h>

class nbsMetadata;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class fmiVisInteractor2D;

namespace fmiVis {


	class ViewportManager;

	class ViewportFactory {
	public:
		static void MakeSingleView(std::string file, nbsMetadata &meta, ViewportManager &viewportMan,
			vtkRenderWindowInteractor *iren, vtkRenderWindow *renWin, fmiVisInteractor2D *style);






	};
}


#endif // ViewportFactory_h__