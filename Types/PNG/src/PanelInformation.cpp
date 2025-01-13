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
    switch (colorType)
    {
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
    switch (interlace)
    {
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

    // size
    general->AddItem({ "Size", tempStr.Format("%s bytes", n.ToString(png->obj->GetData().GetSize(), { NumericFormatFlags::None, 10, 3, ',' }).data()) });

    // Size
    const auto width  = Endian::BigToNative(png->ihdr.width);
    const auto height = Endian::BigToNative(png->ihdr.height);

    general->AddItem({ "Dimension", tempStr.Format("%u x %u", width, height) });
    general->AddItem({ "Bit Depth", tempStr.Format("%u bit per channel", png->ihdr.bitDepth) });
    general->AddItem({ "Color Type", tempStr.Format("%u: %s", png->ihdr.colorType, getColorType(png->ihdr.colorType).GetText()) });
    general->AddItem({ "Compression Method", tempStr.Format("%u", png->ihdr.compression) });
    general->AddItem({ "Filter Method", tempStr.Format("%u", png->ihdr.filter) });
    general->AddItem({ "Interlace Method", tempStr.Format("%u: %s", png->ihdr.interlace, getInterlaceMethod(png->ihdr.interlace).GetText()) });
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
