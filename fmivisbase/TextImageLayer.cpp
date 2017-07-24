#include "TextImageLayer.h"

#include <vtkImageData.h>
#include <vtkTextRendererStringToImage.h>
#include <vtkImageBlend.h>
#include <vtkTextProperty.h>
#include <vtkStdString.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageReslice.h>
#include "vtkTransform.h"

TextImageLayer::TextImageLayer(int x, int y) :
	image( vtkSmartPointer<vtkImageData>::New()),
	textRen ( vtkSmartPointer<vtkTextRendererStringToImage>::New()),
	blend (vtkSmartPointer<vtkImageBlend>::New()),
	textProp (vtkSmartPointer<vtkTextProperty>::New()),
	outData(vtkSmartPointer< vtkImageData>::New()),
	pad(vtkSmartPointer<vtkImageReslice>::New()),
	buffer(vtkSmartPointer<vtkImageData>::New())

{
	textProp->SetLineSpacing(2.17);
	textProp->SetFontFamilyToCourier();
	blend->SetBlendModeToCompound();
	Clear(x,y);
}

TextImageLayer::TextImageLayer(int x, int y, std::string &font):
	TextImageLayer(x,y) 
{
	textProp->SetFontFamily(VTK_FONT_FILE);
	textProp->SetFontFile(font.c_str());
}

TextImageLayer::~TextImageLayer()
{
}

void TextImageLayer::AddText(std::string &s, int x, int y)
{
	vtkStdString text = vtkStdString{ s };

//	auto buffer = vtkSmartPointer<vtkImageData>::New();

	buffer->PrepareForNewData();
	textRen->RenderString(textProp, text,70,buffer);


	int dim[3];
	buffer->GetDimensions(dim);

	int extents[6];

	buffer->GetExtent(extents);
//	buffer->SetOrigin(x, y, 0);
	if (x > 0) {
		extents[1] += x;
		extents[0] += x;
	}
	else
		extents[0] += x;
	if (y > 0) {
		extents[3] += y;
		extents[2] += y;
	}
	else
		extents[2] += y;


	//auto pad = vtkSmartPointer<vtkImageReslice>::New();

	pad->SetOutputExtent(extents);


	pad->RemoveAllInputs();
	pad->AddInputData(buffer);

	pad->Update();

	buffer->ShallowCopy(pad->GetOutput());


	blend->AddInputData( buffer );


	//blend->AddInputData(mask);

}

void TextImageLayer::SetSize(float sz)
{
	textProp->SetFontSize(sz);
}

void TextImageLayer::SetColor(double r, double g, double b, double a)
{
	textProp->SetColor(r, g, b);
	textProp->SetOpacity(a);
}

void TextImageLayer::Clear(int sx, int sy)
{
	image->SetDimensions(sx, sy, 1);
	Clear();
}

void TextImageLayer::Clear() {
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 4);


	blend->RemoveAllInputs();
	blend->SetInputData(image);
}

vtkSmartPointer<vtkImageData> TextImageLayer::GetImage()
{
	blend->Update();
	outData->ShallowCopy(blend->GetOutput());

	auto dim = outData->GetDimensions();

	for (long iy = 0; iy < dim[1]; ++iy) {
		for (long ix = 0; ix < dim[0]; ++ix) {
			auto bp = static_cast<unsigned char*>(outData->GetScalarPointer(ix, iy,0));
			if (bp[0] > 0 || bp[1] > 0 || bp[2] > 0) 
				bp[3] = 255;
		} 
	}
	outData->Modified();

	return outData;
}

