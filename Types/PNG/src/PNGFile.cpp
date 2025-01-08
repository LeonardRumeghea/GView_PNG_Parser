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

    auto& data = this->obj->GetData();

    CHECK(data.Copy<Signature>(0, signature), false, "");
    // check type pt header (return false altfel)
    CHECK(data.Copy<IhdrChunk>(sizeof(Signature), ihdr), false, "");
    // extragem width and hight

    // vedem daca avem date relevante lua din IdatChunk (unul sau mai multe chunkuri)

    uint64 offset = sizeof(Signature) + sizeof(IhdrChunk);
    bool found    = false;
    while (offset < data.GetSize())
    {
        uint32 chunkType;
        CHECK(data.Copy<uint32>(offset, chunkType), false, "");
        //if (PNG::IDAT_CHUNK_TYPE == chunkType) 
        //{
        //    CHECK(data.Copy<IdatChunk>(offset - 4, idat), false, "");
        //}
        //if (PNG::PLTE_CHUNK_TYPE == chunkType) 
        //{
        //    CHECK(data.Copy<PlteChunk>(offset - 4, plte), false, "");
        //}
        if (PNG::IEND_CHUNK_TYPE == chunkType)
        {
            CHECK(data.Copy<IendChunk>(offset - 4, iend), false, "");
            found = true;
            break;
        }
        offset += 1;
    }
    return found;
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