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
    CHECK(data.Copy<IhdrChunk>(sizeof(Signature), ihdr), false, "");

    uint64 offset = sizeof(Signature) + sizeof(IhdrChunk);
    bool found    = false;
    while (offset < data.GetSize())
    {
        uint16 marker;
        CHECK(data.Copy<uint16>(offset, marker), false, "");

        // TODO: Translate the PNG markers

        // get the width and height 
        // if (marker == PNG::JPG_SOF0_MARKER || marker == JPG::JPG_SOF1_MARKER || 
        //     marker == JPG::JPG_SOF2_MARKER || marker == JPG::JPG_SOF3_MARKER)
        // {
        //     CHECK(data.Copy<SOF0MarkerSegment>(offset + 5, sof0MarkerSegment), false, "");
        //     found = true;
        //     break;
        // }
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