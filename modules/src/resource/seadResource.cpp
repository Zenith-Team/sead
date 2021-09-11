#include <filedevice/seadFileDeviceMgr.h>
#include <prim/seadSafeString.h>
#include <resource/seadResource.h>

namespace sead {

Resource::Resource()
    : TListNode<Resource*>(this)
{
}

Resource::~Resource()
{
}

void Resource::doPostCreate_()
{
}

DirectResource::DirectResource()
    : Resource()
    , mRawData(NULL)
    , mRawSize(0)
    , mBufferSize(0)
    , mSettingFlag()
{
}

DirectResource::~DirectResource()
{
    if (mSettingFlag.isOnBit(0))
        delete[] mRawData;
}

s32 DirectResource::getLoadDataAlignment()
{
    return 4;
}

void DirectResource::doCreate_(u8*, u32, Heap*)
{
}

void DirectResource::create(u8* buffer, u32 bufferSize, u32 allocSize, bool allocated, Heap* heap)
{
    if (mRawData != NULL)
    {
        //SEAD_ASSERT_MSG(false, "read twice");
        return;
    }

    mRawSize = bufferSize;
    mBufferSize = allocSize;
    mRawData = buffer;
    mSettingFlag.changeBit(0, allocated);

    return doCreate_(buffer, bufferSize, heap);
}

ResourceFactory::~ResourceFactory()
{
    if (ResourceMgr::instance() != NULL)
        ResourceMgr::instance()->unregisterFactory(this);
}

Resource* DirectResourceFactoryBase::create(const ResourceMgr::CreateArg& createArg)
{
    DirectResource* resource = newResource_(createArg.heap, createArg.alignment);
    if (resource == NULL)
    {
        //SEAD_ASSERT_MSG(false, "resource new failed.");
        return NULL;
    }

    uintptr_t bufferPtr = reinterpret_cast<uintptr_t>(createArg.buffer);
    s32 alignment = resource->getLoadDataAlignment();
    if (bufferPtr % alignment != 0)
    {
        //SEAD_ASSERT_MSG(false, "buffer alignment invalid: %p, %d", createArg.buffer, alignment);
        delete resource;
        return NULL;
    }

    resource->create(createArg.buffer, createArg.file_size, createArg.buffer_size, createArg.need_unload, createArg.heap);
    return resource;
}

Resource* DirectResourceFactoryBase::tryCreate(const ResourceMgr::LoadArg& loadArg)
{
    DirectResource* resource = newResource_(loadArg.instance_heap, loadArg.instance_alignment);
    if (resource == NULL)
    {
        //SEAD_ASSERT_MSG(false, "resource new failed.");
        return NULL;
    }

    FileDevice::LoadArg fileLoadArg;
    u8* data;

    fileLoadArg.path = loadArg.path;
    fileLoadArg.buffer = loadArg.load_data_buffer;
    fileLoadArg.buffer_size = loadArg.load_data_buffer_size;
    fileLoadArg.heap = loadArg.load_data_heap;
    fileLoadArg.div_size = loadArg.div_size;

    if (loadArg.load_data_alignment != 0)
        fileLoadArg.alignment = loadArg.load_data_alignment;

    else
        fileLoadArg.alignment = ((loadArg.instance_alignment < 0)? -1: 1) * resource->getLoadDataAlignment();

    if (loadArg.device != NULL)
        data = loadArg.device->tryLoad(fileLoadArg);

    else
        data = FileDeviceMgr::instance()->tryLoad(fileLoadArg);

    if (data == NULL)
    {
        delete resource;
        return NULL;
    }

    resource->create(data, fileLoadArg.read_size, fileLoadArg.roundup_size, fileLoadArg.need_unload, loadArg.instance_heap);
    return resource;
}

Resource*
DirectResourceFactoryBase::tryCreateWithDecomp(
    const ResourceMgr::LoadArg& loadArg, Decompressor* decompressor
)
{
    DirectResource* resource = newResource_(loadArg.instance_heap, loadArg.instance_alignment);
    if (resource == NULL)
    {
        //SEAD_ASSERT_MSG(false, "resource new failed.");
        return NULL;
    }

    u32 outSize = 0;
    u32 outAllocSize = 0;
    bool outAllocated = false;

    u8* data = decompressor->tryDecompFromDevice(loadArg, resource, &outSize, &outAllocSize, &outAllocated);

    resource->create(data, outSize, outAllocSize, outAllocated, loadArg.instance_heap);
    return resource;
}

}
