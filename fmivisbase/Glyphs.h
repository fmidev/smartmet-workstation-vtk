#ifndef Glyphs_h
#define Glyphs_h

#include <vtkSmartPointer.h>

class vtkGlyph3D;

void SetSourceBarb(vtkGlyph3D *glyph,float size);

void SetSourceArrow(vtkGlyph3D *glyph, float size);


#endif // Glyphs_h
