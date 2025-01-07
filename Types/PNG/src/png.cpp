#include "png.hpp"

using namespace AppCUI;
using namespace AppCUI::Utils;
using namespace AppCUI::Application;
using namespace AppCUI::Controls;
using namespace GView::Utils;
using namespace GView::Type;
using namespace GView;
using namespace GView::View;

extern "C"
{

PLUGIN_EXPORT bool Validate(const AppCUI::Utils::BufferView& buf, const std::string_view& extension)
{
    // TODO: Check if are there any additional checks that can be done here
    if (buf.GetLength() < sizeof(PNG::Signature) + sizeof(PNG::IhdrChunk)) {
    return false;
    }

    auto signature = buf.GetObject<PNG::Signature>();
    if (memcmp(signature->signature, PNG::PNG_SIGNATURE, 8) != 0) {
        return false;
    }

    auto ihdrChunk = buf.GetObject<PNG::IhdrChunk>(sizeof(PNG::Signature));
    if (ihdrChunk->chunkType != PNG::IHDR_CHUNK_TYPE) {
        return false;
    }

    // Additional validation for IHDR fields can be added here if necessary
    return true;
}
    PLUGIN_EXPORT TypeInterface* CreateInstance()
    {
        return new PNG::PNGFile;
    }

    void CreateBufferView(Reference<GView::View::WindowInterface> win, Reference<PNG::PNGFile> png)
    {
        // TODO: Implementation of the buffer viewer
        BufferViewer::Settings settings;

        const std::vector<ColorPair> colors = { ColorPair{ Color::Teal, Color::DarkBlue }, ColorPair{ Color::Yellow, Color::DarkBlue } };

        auto& data            = png->obj->GetData();
        const uint64 dataSize = data.GetSize();
        uint64 offset         = 0;
        uint32 colorIndex     = 0;
        uint32 segmentCount   = 1;

        settings.AddZone(0, sizeof(PNG::Signature), ColorPair{ Color::Magenta, Color::DarkBlue }, "PNG Signature");
        offset += sizeof(PNG::Signature);

        settings.AddZone(offset, sizeof(PNG::IhdrChunk), ColorPair{ Color::Olive, Color::DarkBlue }, "IHDR Chunk");
        offset += sizeof(PNG::IhdrChunk);
            
    }

    void CreateImageView(Reference<GView::View::WindowInterface> win, Reference<PNG::PNGFile> png)
    {
        GView::View::ImageViewer::Settings settings;
        settings.SetLoadImageCallback(png.ToBase<View::ImageViewer::LoadImageInterface>());
        settings.AddImage(0, png->obj->GetData().GetSize());
        win->CreateViewer(settings);
    }

    PLUGIN_EXPORT bool PopulateWindow(Reference<GView::View::WindowInterface> win)
    {
        auto png = win->GetObject()->GetContentType<PNG::PNGFile>();
        png->Update();

        // add viewer
        CreateImageView(win, png);
        CreateBufferView(win, png);

        // add panels
        win->AddPanel(Pointer<TabPage>(new PNG::Panels::Information(png)), true);

        return true;
    }

    PLUGIN_EXPORT void UpdateSettings(IniSection sect)
    {
        sect["Pattern"]     = "magic:89 50 4E 47 0D 0A 1A 0A";
        sect["Priority"]    = 1;
        sect["Description"] = "PNG image file (*.png)";
    }
}

int main()
{
    return 0;
}