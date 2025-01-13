#include "png.hpp"

using namespace GView::Type::PNG;

PNGFile::PNGFile()
{
}

bool PNGFile::Update()
{
    memset(&signature, 0, sizeof(signature));
    memset(&ihdr, 0, sizeof(ihdr));
    memset(&plte, 0, sizeof(plte));
    memset(&idat, 0, sizeof(idat));
    memset(&iend, 0, sizeof(iend));

    auto& data    = this->obj->GetData();
    uint64 offset = 0;
    bool found    = false;
    
    // Get the PNG Signature and IHDR chunk
    CHECK(data.Copy<Signature>(offset, signature), false, "");
    offset += sizeof(Signature);

    CHECK(data.Copy<IhdrChunk>(offset, ihdr), false, "");
    offset += sizeof(IhdrChunk);
    
    // extragem width and hight

    // vedem daca avem date relevante lua din IdatChunk (unul sau mai multe chunkuri)
    // while (offset < data.GetSize())
    // {
    //     uint32 chunkType;
    //     CHECK(data.Copy<uint32>(offset, chunkType), false, "");
        //if (PNG::IDAT_CHUNK_TYPE == chunkType) 
        //{
        //    CHECK(data.Copy<IdatChunk>(offset - 4, idat), false, "");
        //}
        //if (PNG::PLTE_CHUNK_TYPE == chunkType) 
        //{
        //    CHECK(data.Copy<PlteChunk>(offset - 4, plte), false, "");
        //}
    //     if (PNG::IEND_CHUNK_TYPE == chunkType)
    //     {
    //         CHECK(data.Copy<IendChunk>(offset - 4, iend), false, "");
    //         found = true;
    //         break;
    //     }
    //     offset += 1;
    // }

    // offset = sizeof(PNG::Signature) + sizeof(PNG::IhdrChunk);

    // while (offset < bufSize)
    // {
    //     if (memcmp(data + offset, PNG::IDAT_CHUNK, sizeof(PNG::IDAT_CHUNK)) == 0) {
    //         foundIDAT = true;
    //     }

    //     if (memcmp(data + offset, PNG::IEND_CHUNK, sizeof(PNG::IEND_CHUNK)) == 0) {
    //         const uint8 sizeZero[] = { 0, 0, 0, 0 };
    //         if (memcmp(data + offset - sizeof(PNG::IEND_CHUNK), sizeZero, sizeof(sizeZero))== 0) {
    //             foundIEND = true;
    //         }
    //     }

    //     if (foundIDAT && foundIEND) {
    //         break;
    //     }

    //     offset++;
    // }

    return true;
}

bool PNGFile::LoadImageToObject(Image& img, uint32 index)
{
    Buffer buf;
    auto bf = obj->GetData().GetEntireFile();
    if (bf.IsValid() == false) {
        buf = this->obj->GetData().CopyEntireFile();
        CHECK(buf.IsValid(), false, "Fail to copy Entire file");
        bf = (BufferView) buf;
    }
    CHECK(img.Create(bf), false, "");

    return true;
}