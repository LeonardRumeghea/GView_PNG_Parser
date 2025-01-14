#include "png.hpp"

using namespace GView::Type::PNG;
using namespace AppCUI::Controls;

Panels::Information::Information(Reference<GView::Type::PNG::PNGFile> _png) : TabPage("&Information")
{
    png     = _png;
    general = Factory::ListView::Create(this, "x:0,y:0,w:100%,h:10", { "n:Field,w:12", "n:Value,w:100" }, ListViewFlags::None);
    issues  = Factory::ListView::Create(this, "x:0,y:21,w:100%,h:10", { "n:Info,w:200" }, ListViewFlags::HideColumns);

    this->Update();
}

String getColorType(uint8_t colorType)
{
    switch (colorType) {
    case 0:
        return String("Grayscale");
    case 2:
        return String("Truecolor");
    case 3:
        return String("Indexed-color");
    case 4:
        return String("Grayscale with alpha");
    case 6:
        return String("Truecolor with alpha");
    default:
        return String("Unknown");
    }
}

String getInterlaceMethod(uint8_t interlace)
{
    switch (interlace) {
    case 0:
        return String("No interlace");
    case 1:
        return String("Adam7 interlace");
    default:
        return String("Unknown");
    }
}

void Panels::Information::UpdateGeneralInformation()
{
    LocalString<256> tempStr;
    NumericFormatter n;

    general->DeleteAllItems();
    general->AddItem("File");

    // Size of the PNG file
    general->AddItem({ "Size", tempStr.Format("%s bytes", n.ToString(png->obj->GetData().GetSize(), { NumericFormatFlags::None, 10, 3, ',' }).data()) });

    // Information about the PNG file from the IHDR chunk
    general->AddItem("PNG Information");

    const auto width  = Endian::BigToNative(png->ihdr.width);
    const auto height = Endian::BigToNative(png->ihdr.height);
    general->AddItem({ "Dimension", tempStr.Format("%u x %u", width, height) });

    const auto bitDepth = png->ihdr.bitDepth;
    general->AddItem({ "Bit Depth", tempStr.Format("%u bit per channel", bitDepth) });
    
    const auto colorType = png->ihdr.colorType;
    const auto colorTypeStr = getColorType(colorType);
    general->AddItem({ "Color Type", tempStr.Format("%u: %s", colorType, colorTypeStr.GetText()) });

    const auto compressionMethod = png->ihdr.compression;
    general->AddItem({ "Compression Method", tempStr.Format("%u", compressionMethod) });

    const auto filterMethod = png->ihdr.filter;
    general->AddItem({ "Filter Method", tempStr.Format("%u", filterMethod) });

    const auto interlaceMethod = png->ihdr.interlace;
    const auto interlaceMethodStr = getInterlaceMethod(interlaceMethod);
    general->AddItem({ "Interlace Method", tempStr.Format("%u: %s", interlaceMethod, interlaceMethodStr.GetText()) });
}

void Panels::Information::UpdateIssues()
{
}

void Panels::Information::RecomputePanelsPositions()
{
    int py = 0;
    int w  = this->GetWidth();
    int h  = this->GetHeight();

    if ((!general.IsValid()) || (!issues.IsValid())) {
        return;
    }

    issues->SetVisible(false);
    this->general->Resize(w, h);
}

void Panels::Information::Update()
{
    UpdateGeneralInformation();
    UpdateIssues();
    RecomputePanelsPositions();
}
