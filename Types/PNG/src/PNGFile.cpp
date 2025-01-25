#include "png.hpp"

using namespace GView::Type::PNG;

PNGFile::PNGFile()
{
}

bool PNGFile::Update()
{
    memset(&signature, 0, sizeof(signature));
    memset(&ihdr,      0, sizeof(ihdr));
    // memset(&plte,      0, sizeof(plte));
    // memset(&idat,      0, sizeof(idat));
    // memset(&iend,      0, sizeof(iend));

    auto& data    = this->obj->GetData();
    uint64 offset = 0;

    // Get and save the PNG signature and IHDR chunks from the file into object attributes
    CHECK(data.Copy<Signature>(offset, signature), false, "");
    offset += sizeof(Signature);

    CHECK(data.Copy<IhdrChunk>(offset, ihdr), false, "");
    offset += sizeof(IhdrChunk);

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
