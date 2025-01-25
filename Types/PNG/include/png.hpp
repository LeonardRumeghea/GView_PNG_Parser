#pragma once

#include "GView.hpp"

namespace GView
{

namespace Type
{
    namespace PNG
    {

#pragma pack(push, 1)
        
        #define reverseBytes32(x) (((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) | ((x & 0x00FF0000) >> 8) | ((x & 0xFF000000) >> 24))

        #define ADD_ZONE(desc) settings.AddZone(offset, chunkSize, colors[colorIndex++], (desc))

        constexpr uint8_t PNG_SIGNATURE[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
        constexpr uint8_t IHDR_CHUNK[4]    = { 0x49, 0x48, 0x44, 0x52 }; // 'IHDR'
        constexpr uint8_t IDAT_CHUNK[4]    = { 0x49, 0x44, 0x41, 0x54 }; // 'IDAT'
        constexpr uint8_t IEND_CHUNK[4]    = { 0x49, 0x45, 0x4E, 0x44 }; // 'IEND'

        constexpr uint32_t IHDR_CHUNK_TYPE = 0x52444849; // 'IHDR' -> 49 48 44 52
        constexpr uint32_t SRGB_CHUNK_TYPE = 0x42475273; // 'sRGB' -> 73 52 47 42
        constexpr uint32_t IDAT_CHUNK_TYPE = 0x54414449; // 'IDAT' -> 49 44 41 54
        constexpr uint32_t IEND_CHUNK_TYPE = 0x444E4549; // 'IEND' -> 49 45 4E 44
        constexpr uint32_t PLTE_CHUNK_TYPE = 0x45544C50; // 'PLTE' -> 50 4C 54 45
        constexpr uint32_t CHRM_CHUNK_TYPE = 0x4D524863; // 'cHRM' -> 63 48 52 4D
        constexpr uint32_t GAMA_CHUNK_TYPE = 0x414D4167; // 'gAMA' -> 67 41 4D 41
        constexpr uint32_t ICCP_CHUNK_TYPE = 0x50434369; // 'iCCP' -> 69 43 43 50
        constexpr uint32_t SBIT_CHUNK_TYPE = 0x54494273; // 'sBIT' -> 73 42 49 54
        constexpr uint32_t BKGD_CHUNK_TYPE = 0x44474B62; // 'bKGD' -> 62 4B 47 44
        constexpr uint32_t HIST_CHUNK_TYPE = 0x54534968; // 'hIST' -> 68 49 53 54
        constexpr uint32_t TRNS_CHUNK_TYPE = 0x534E5274; // 'tRNS' -> 74 52 4E 53
        constexpr uint32_t PHYS_CHUNK_TYPE = 0x73594870; // 'pHYs' -> 70 48 59 73
        constexpr uint32_t SPLT_CHUNK_TYPE = 0x544C5073; // 'sPLT' -> 73 50 4C 54
        constexpr uint32_t TIME_CHUNK_TYPE = 0x454D4974; // 'tIME' -> 74 49 4D 45
        constexpr uint32_t TEXT_CHUNK_TYPE = 0x74584574; // 'tEXt' -> 74 45 58 74
        constexpr uint32_t ZTXT_CHUNK_TYPE = 0x7458547A; // 'zTXt' -> 7A 54 58 74
        constexpr uint32_t ITXT_CHUNK_TYPE = 0x74585469; // 'iTXt' -> 69 54 58 74

        constexpr uint8_t CRC_SIZE          = 4; // Size of the CRC field
        constexpr uint8_t CHUNK_LENGTH_SIZE = 4; // Size of the length field
        constexpr uint8_t CHUNK_TYPE_SIZE   = 4; // Size of the chunk type field


        struct Signature {
            uint8_t signature[8];
        };

        struct IhdrChunk {
            uint32 length;      // Length of the IHDR chunk data
            uint8 chunkType[4]; // Chunk type, should be 'IHDR'
            uint32 width;       // Image width in pixels
            uint32 height;      // Image height in pixels
            uint8 bitDepth;     // Bit depth
            uint8 colorType;    // Color type
            uint8 compression;  // Compression method
            uint8 filter;       // Filter method
            uint8 interlace;    // Interlace method
            uint32 crc;         // CRC for the IHDR chunk
        };

        struct sRgbChunk {
            uint32 length;         // Length of the sRGB chunk data (1 byte)
            uint8 chunkType[4];    // Chunk type, should be 'sRGB'
            uint8 renderingIntent; // Rendering intent
            uint32 crc;            // CRC for the sRGB chunk
        };

        struct PlteChunk {
            uint32 length;      // Length of the PLTE chunk data
            uint8 chunkType[4]; // Chunk type, should be 'PLTE'
            uint8* palette;     // Palette entries
            uint32 crc;         // CRC for the PLTE chunk
        };

        struct IdatChunk {
            uint32 length;      // Length of the IDAT chunk data
            uint8 chunkType[4]; // Chunk type, should be 'IDAT'
            uint8* data;        // Compressed image data
            uint32 crc;         // CRC for the IDAT chunk
        };

        struct IendChunk {
            uint32 length;      // Length of the IEND chunk data (always 0)
            uint8 chunkType[4]; // Chunk type, should be 'IEND'
            uint32 crc;         // CRC for the IEND chunk
        };

#pragma pack(pop) // Back to default packing

        class PNGFile : public TypeInterface, public View::ImageViewer::LoadImageInterface
        {
          public:
            Signature signature;
            IhdrChunk ihdr;
            // sRgbChunk srgb;
            // PlteChunk plte;
            // std::list<IdatChunk> idat;
            // IendChunk iend;

            Reference<GView::Utils::SelectionZoneInterface> selectionZoneInterface;

          public:
            PNGFile();
            virtual ~PNGFile()
            {
            }

            bool Update();

            std::string_view GetTypeName() override
            {
                return "PNG";
            }

            void RunCommand(std::string_view) override
            {
            }

            bool LoadImageToObject(Image& img, uint32 index) override;

            uint32 GetSelectionZonesCount() override
            {
                CHECK(selectionZoneInterface.IsValid(), 0, "");
                return selectionZoneInterface->GetSelectionZonesCount();
            }

            TypeInterface::SelectionZone GetSelectionZone(uint32 index) override
            {
                static auto zone = TypeInterface::SelectionZone{ 0, 0 };
                CHECK(selectionZoneInterface.IsValid(), zone, "");
                CHECK(index < selectionZoneInterface->GetSelectionZonesCount(), zone, "");

                return selectionZoneInterface->GetSelectionZone(index);
            }

            bool UpdateKeys(KeyboardControlsInterface* interface) override
            {
                return true;
            }
        };

        namespace Panels
        {
            class Information : public AppCUI::Controls::TabPage
            {
                Reference<GView::Type::PNG::PNGFile> png;
                Reference<AppCUI::Controls::ListView> general;
                Reference<AppCUI::Controls::ListView> issues;

                void UpdateGeneralInformation();
                void UpdateIssues();
                void RecomputePanelsPositions();

              public:
                Information(Reference<GView::Type::PNG::PNGFile> png);

                void Update();
                virtual void OnAfterResize(int newWidth, int newHeight) override
                {
                    RecomputePanelsPositions();
                }
            };

        }; // namespace Panels

    } // namespace PNG

} // namespace Type

} // namespace GView