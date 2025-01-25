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
    // Check if the buffer is large enough to contain the PNG signature, IHDR chunk, at least one IDAT chunk and the IEND chunk
    const uint8 minSize = sizeof(PNG::Signature) + sizeof(PNG::IhdrChunk) + sizeof(PNG::IdatChunk) + sizeof(PNG::IendChunk);
    if (buf.GetLength() < minSize) {
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

    // Check if there is at least one IDAT (Image Data) chunk. This is done by searching for the IDAT chunk type
    // The IDAT chunk is the compressed image data and it appears among the first chunks in the file
    uint64 offset     = sizeof(PNG::Signature) + sizeof(PNG::IhdrChunk);
    uint64 bufSize    = buf.GetLength();
    const uint8* data = buf.GetData();
    bool foundIDAT    = false;

    while (!foundIDAT && offset < bufSize) {
        if (memcmp(data + offset, PNG::IDAT_CHUNK, PNG::CHUNK_TYPE_SIZE) == 0) {
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

    auto& data            = png->obj->GetData();
    const uint64 dataSize = data.GetSize();
    uint64 offset         = 0;
    uint8 colorIndex      = 0;

    const ColorPair unknownColor        = ColorPair{ Color::Red, Color::DarkBlue };
    const std::vector<ColorPair> colors = { ColorPair{ Color::Teal, Color::DarkBlue },  ColorPair{ Color::Yellow, Color::DarkBlue },
                                            ColorPair{ Color::Green, Color::DarkBlue }, ColorPair{ Color::Pink, Color::DarkBlue },
                                            ColorPair{ Color::Blue, Color::DarkBlue },  ColorPair{ Color::Magenta, Color::DarkBlue },
                                            ColorPair{ Color::Olive, Color::DarkBlue }, ColorPair{ Color::Silver, Color::DarkBlue },
                                            ColorPair{ Color::White, Color::DarkBlue }, ColorPair{ Color::Black, Color::DarkBlue } };

    const auto colorCount = colors.size();

    settings.AddZone(0, sizeof(PNG::Signature), colors[colorIndex++], "PNG Signature");
    offset += sizeof(PNG::Signature);

    settings.AddZone(offset, sizeof(PNG::IhdrChunk), colors[colorIndex++], "IHDR Chunk");
    offset += sizeof(PNG::IhdrChunk);

    // return;

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
        if (!data.Copy<uint32>(offset + PNG::CHUNK_TYPE_SIZE, chunkType)) {
            break;
        }

        // Compute the size of the chunk based on the length of the chunk data, which is variable and the size of the chunk type, 
        // length and CRC fields (4 + 4 + 4 = 12 bytes)
        chunkSize = PNG::CHUNK_LENGTH_SIZE + PNG::CHUNK_TYPE_SIZE + chunkLength + PNG::CRC_SIZE;

        switch (chunkType) {
        case PNG::SRGB_CHUNK_TYPE:
            // ADD_ZONE("sRGB Chunk");
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "sRGB Chunk");
            break;

        case PNG::PLTE_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "PLTE Chunk");
            break;

        case PNG::IDAT_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "IDAT Chunk");
            break;

        case PNG::IEND_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "IEND Chunk");
            break;

        case PNG::CHRM_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "cHRM Chunk");
            break;

        case PNG::GAMA_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "gAMA Chunk");
            break;

        case PNG::ICCP_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "iCCP Chunk");
            break;

        case PNG::SBIT_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "sBIT Chunk");
            break;

        case PNG::BKGD_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "bKGD Chunk");
            break;

        case PNG::HIST_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "hIST Chunk");
            break;

        case PNG::TRNS_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "tRNS Chunk");
            break;

        case PNG::PHYS_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "pHYs Chunk");
            break;

        case PNG::SPLT_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "sPLT Chunk");
            break;

        case PNG::TIME_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "tIME Chunk");
            break;

        case PNG::TEXT_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "tEXt Chunk");
            break;

        case PNG::ZTXT_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "zTXt Chunk");
            break;

        case PNG::ITXT_CHUNK_TYPE:
            settings.AddZone(offset, chunkSize, colors[colorIndex++], "iTXt Chunk");
            break;

        default:
            chunkSize = 1;
            settings.AddZone(offset, chunkSize, unknownColor, "Unknown Chunk");
            break;
        }

        offset += chunkSize;
        colorIndex %= colorCount;
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

    // Add viewer
    CreateImageView(win, png);
    CreateBufferView(win, png);

    // Add panels
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
