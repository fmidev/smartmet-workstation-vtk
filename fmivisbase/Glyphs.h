#ifndef Glyphs_h
#define Glyphs_h

#include <vtkSmartPointer.h>

class vtkGlyph3D;

//meshes for glyphs are generated here

namespace fmiVis {

	void SetSourceBarb(vtkGlyph3D *glyph, float size);

	void SetSourceArrow(vtkGlyph3D *glyph, float size);

}
#endif // Glyphs_h
