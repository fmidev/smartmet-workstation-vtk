#ifndef TextImageLayer_h__
#define TextImageLayer_h__

#include <vtkSmartPointer.h>

class vtkImageData;
class vtkTextRendererStringToImage;
class vtkImageBlend;
class vtkTextProperty;
class vtkImageAppendComponents;
class vtkImageReslice;

class TextImageLayer {

	vtkSmartPointer<vtkImageData> image;
	vtkSmartPointer<vtkImageData> buffer;

	vtkSmartPointer<vtkImageReslice> pad;

	vtkSmartPointer<vtkTextRendererStringToImage> textRen;

	vtkSmartPointer<vtkImageBlend> blend;

	vtkSmartPointer<vtkTextProperty> textProp;

	vtkSmartPointer< vtkImageData > outData;

public:
	TextImageLayer(int x, int y);
	TextImageLayer(int x, int y,std::string &font);
	~TextImageLayer();

	void AddText(std::string &s,int x, int y);

	void SetSize(float sz);
	void SetColor(double r, double g, double b, double a = 1.0);

	void Clear(int sx, int sy);
	void Clear();

	vtkSmartPointer<vtkImageData> GetImage();
};

#endif // TextImageLayer_h__
