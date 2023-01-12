#include "zip.hpp"

#include <queue>
#include <map>

namespace GView::Type::ZIP
{

ZIPFile::ZIPFile()
{
}

bool ZIPFile::Update()
{
    CHECK(GView::ZIP::GetInfo(obj->GetPath(), this->info), false, "");
    return true;
}

bool ZIPFile::BeginIteration(std::u16string_view path, AppCUI::Controls::TreeViewItem parent)
{
    currentItemIndex = 0;
    return this->info.GetCount() > 0;
}

bool ZIPFile::PopulateItem(TreeViewItem item)
{
    GView::ZIP::Entry entry{ 0 };
    CHECK(this->info.GetEntry(currentItemIndex, entry), false, "");

    const auto filename = entry.GetFilename();
    item.SetText(filename);

    currentItemIndex++;

    return currentItemIndex != this->info.GetCount();
}

void ZIPFile::OnOpenItem(std::u16string_view path, AppCUI::Controls::TreeViewItem item)
{
    CHECKRET(item.GetParent().GetHandle() != InvalidItemHandle, "");

    // auto data         = item.GetData<ECMA_119_DirectoryRecord>();
    // const auto offset = (uint64) data->locationOfExtent.LSB * pvd.vdd.logicalBlockSize.LSB;
    // const auto length = (uint32) data->dataLength.LSB;
    // const auto name   = std::string_view{ data->fileIdentifier, data->lengthOfFileIdentifier };
    // const auto buffer = obj->GetData().CopyToBuffer(offset, length);
    //
    // GView::App::OpenBuffer(buffer, name, name, GView::App::OpenMethod::BestMatch);
}
} // namespace GView::Type::ZIP
