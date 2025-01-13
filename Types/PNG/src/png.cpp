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

extern "C" {

PLUGIN_EXPORT bool Validate(const AppCUI::Utils::BufferView& buf, const std::string_view& extension)
{
    // Issue #1: The buffer is not large enough??

    uint64 offset = 0;
    uint64 bufSize = buf.GetLength();
    const uint8* data = buf.GetData();

    bool foundIDAT = false;

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
    if (memcmp(ihdrChunk->chunkType, PNG::IHDR_CHUNK, sizeof(PNG::IHDR_CHUNK)) != 0) {
        return false;
    }

    // return true;

    offset = sizeof(PNG::Signature) + sizeof(PNG::IhdrChunk);

    while (!foundIDAT && offset < bufSize) {
        if (memcmp(data + offset, PNG::IDAT_CHUNK, sizeof(PNG::IDAT_CHUNK)) == 0) {
            foundIDAT = true;
        }

        offset++;
    }

    return foundIDAT;
}

PLUGIN_EXPORT TypeInterface* CreateInstance()
{
    return new PNG::PNGFile;
}

void CreateBufferView(Reference<GView::View::WindowInterface> win, Reference<PNG::PNGFile> png)
{
    BufferViewer::Settings settings;

    auto& data             = png->obj->GetData();
    const uint64 dataSize  = data.GetSize();
    uint64 offset          = 0;
    uint8 colorIndex       = 0;
    const uint8 colorCount = 10;

    const ColorPair unknownColor        = ColorPair{ Color::Red, Color::DarkBlue };
    const std::vector<ColorPair> colors = { ColorPair{ Color::Teal, Color::DarkBlue },  ColorPair{ Color::Yellow, Color::DarkBlue },
                                            ColorPair{ Color::Green, Color::DarkBlue }, ColorPair{ Color::Pink, Color::DarkBlue },
                                            ColorPair{ Color::Blue, Color::DarkBlue },  ColorPair{ Color::Magenta, Color::DarkBlue },
                                            ColorPair{ Color::Olive, Color::DarkBlue }, ColorPair{ Color::Silver, Color::DarkBlue },
                                            ColorPair{ Color::White, Color::DarkBlue }, ColorPair{ Color::Black, Color::DarkBlue } };


    settings.AddZone(0, sizeof(PNG::Signature), colors[colorIndex++ % colorCount], "PNG Signature");
    offset += sizeof(PNG::Signature);

    settings.AddZone(offset, sizeof(PNG::IhdrChunk), colors[colorIndex++ % colorCount], "IHDR Chunk");
    offset += sizeof(PNG::IhdrChunk);

    while (offset < dataSize) {
        uint32 chunkLength;
        uint32 chunkType;
        uint32 chunkSize;

        // The first 4 bytes of a chunk are the length of the chunk data
        if (!data.Copy<uint32>(offset, chunkLength)) {
            break;
        }

        // The size bytes are in the opposite order in the file. We need to reverse them to get the actual size
        chunkLength = reverseBytes32(chunkLength);

        // The next 4 bytes are the chunk type (e.g. IHDR, IDAT, IEND, sRGB, etc.)
        if (!data.Copy<uint32>(offset + sizeof(chunkLength), chunkType)) {
            break;
        }

        // Compute the size of the chunk based on the length of the chunk data, which is variable and the size of the chunk type, length and CRC fields (4 + 4 + 4 = 12 bytes)
        chunkSize = sizeof(chunkLength) + sizeof(chunkType) + chunkLength + PNG::CRC_SIZE;

        switch (chunkType) {
        case PNG::sRGB_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "sRGB Chunk");
            break;

        case PNG::PLTE_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "PLTE Chunk");
            break;

        case PNG::IDAT_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "IDAT Chunk");
            break;

        case PNG::IEND_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "IEND Chunk");
            break;

        case PNG::cHRM_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "cHRM Chunk");
            break;

        case PNG::gAMA_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "gAMA Chunk");
            break;

        case PNG::iCCP_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "iCCP Chunk");
            break;

        case PNG::sBIT_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "sBIT Chunk");
            break;

        case PNG::bKGD_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "bKGD Chunk");
            break;

        case PNG::hIST_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "hIST Chunk");
            break;

        case PNG::tRNS_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "tRNS Chunk");
            break;

        case PNG::pHYs_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "pHYs Chunk");
            break;

        case PNG::sPLT_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "sPLT Chunk");
            break;

        case PNG::tIME_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "tIME Chunk");
            break;

        case PNG::tEXt_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "tEXt Chunk");
            break;

        case PNG::zTXt_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "zTXt Chunk");
            break;

        case PNG::iTXt_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++ % colorCount], "iTXt Chunk");
            break;

        default:
            settings.AddZone(offset, chunkSize, unknownColor, "Unknown Chunk");
            chunkSize = 1;
            break;
        }

        offset += chunkSize;
    }

    // If there is any trailing data after the last chunk, we will add it to the buffer viewer
    if (offset < dataSize) {
        settings.AddZone(offset, dataSize - offset, unknownColor, "Trailing Data");
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
