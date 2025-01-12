#include "png.hpp"
#include <fstream>

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
    // Check if the buffer is large enough to contain the PNG signature and the IHDR chunk
    if (buf.GetLength() < sizeof(PNG::Signature) + sizeof(PNG::IhdrChunk)) {
        return false;
    }

    // Check if the first 8 bytes are the PNG signature 
    auto signature = buf.GetObject<PNG::Signature>();
    if (memcmp(signature->signature, PNG::PNG_SIGNATURE, sizeof(PNG::PNG_SIGNATURE)) != 0) {
        return false;
    }

    // Check if the next chunk is the IHDR (Image Header) chunk. This is done by checking the chunk type
    auto ihdrChunk = buf.GetObject<PNG::IhdrChunk>(sizeof(PNG::Signature));
    if (memcmp(ihdrChunk->chunkType, PNG::IHDR_CHUNK_TYPE, sizeof(PNG::IHDR_CHUNK_TYPE)) != 0) {
        return false;
    }
    
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

        while (offset < dataSize - PNG::CHUNK_SIZE) {
            uint32 chunkLength;
            uint32 chunkType;

            if (!data.Copy<uint32>(offset, chunkLength)) {
                break;
            }

            if (!data.Copy<uint32>(offset + 4, chunkType)) {
                break;
            }

            if (chunkType == PNG::IDAT_CHUNK_TYPE) {
                settings.AddZone(offset, PNG::CHUNK_SIZE, ColorPair{ Color::Green, Color::DarkBlue }, "IDAT Chunk");
                offset += PNG::CHUNK_SIZE;
            } else {
                break;
            }

            segmentCount++;
        }

        if (offset < dataSize) {
            settings.AddZone(offset, dataSize - offset, ColorPair{ Color::Red, Color::DarkBlue }, "Trailing Data");
        }

        png->selectionZoneInterface = win->GetSelectionZoneInterfaceFromViewerCreation(settings);
            
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