#pragma once 

#include "GView.hpp"

namespace GView
{

namespace Type
{
	namespace PNG
	{

#pragma pack(push, 2)

    const uint8_t PNG_SIGNATURE[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    const uint32_t IHDR_CHUNK_TYPE = 0x49484452; // 'IHDR'
    const uint32 IDAT_CHUNK_TYPE = 0x49444154; // 'IDAT'
    const uint32 IEND_CHUNK_TYPE = 0x49454E44; // 'IEND'
    const uint32 PLTE_CHUNK_TYPE = 0x504C5445; // 'PLTE'

    struct Signature {
        uint8_t signature[8];
    };

    struct IhdrChunk {
        uint32 length;    // Length of the IHDR chunk data
        uint32 chunkType; // Chunk type, should be 'IHDR'
        uint32 width;     // Image width in pixels
        uint32 height;    // Image height in pixels
        uint8 bitDepth;   // Bit depth
        uint8 colorType;  // Color type
        uint8 compression;// Compression method
        uint8 filter;     // Filter method
        uint8 interlace;  // Interlace method
        uint32 crc;       // CRC for the IHDR chunk
    };

    struct PlteChunk {
        uint32 length;    // Length of the PLTE chunk data
        uint32 chunkType; // Chunk type, should be 'PLTE'
        uint8* palette;   // Palette entries
        uint32 crc;       // CRC for the PLTE chunk
    };

    struct IdatChunk {
        uint32 length;    // Length of the IDAT chunk data
        uint32 chunkType; // Chunk type, should be 'IDAT'
        uint8* data;      // Compressed image data
        uint32 crc;       // CRC for the IDAT chunk
    };

    struct IendChunk {
        uint32 length;    // Length of the IEND chunk data (always 0)
        uint32 chunkType; // Chunk type, should be 'IEND'
        uint32 crc;       // CRC for the IEND chunk
    };

#pragma pack(pop) // Back to default packing

        class PNGFile : public TypeInterface, public View::ImageViewer::LoadImageInterface
        {
          public:
            Signature signature;
            IhdrChunk ihdr;
            PlteChunk* plte;
            IdatChunk* idat;
            IendChunk iend;

            Reference<GView::Utils::SelectionZoneInterface> selectionZoneInterface;

          public:
            PNGFile();
            virtual ~PNGFile() { }

            bool Update();

            std::string_view GetTypeName() override
			{
                return "PNG";
			}

            void RunCommand(std::string_view) override { }

            bool LoadImageToObject(Image& img, uint32 index) override;

            uint32 GetSelectionZonesCount() override
			{
                CHECK(selectionZoneInterface.IsValid(), 0, "");
                return selectionZoneInterface->GetSelectionZonesCount();
			}

            TypeInterface::SelectionZone GetSelectionZone(uint32 index) override
			{
                static auto d = TypeInterface::SelectionZone{ 0, 0 };
                CHECK(selectionZoneInterface.IsValid(), d, "");
                CHECK(index < selectionZoneInterface->GetSelectionZonesCount(), d, "");

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