#ifndef SEAD_DECOMPRESSOR_H_
#define SEAD_DECOMPRESSOR_H_

#include <container/seadTList.h>
#include <heap/seadDisposer.h>
#include <prim/seadSafeString.h>
#include <resource/seadResource.h>
#include <resource/seadResourceMgr.h>

namespace sead {

class Decompressor : public TListNode<Decompressor*>, public IDisposer
{
public:
    Decompressor(const SafeString& name)
        : TListNode<Decompressor*>(this)
        , IDisposer()
        , mExt(name)
    {
    }

    virtual ~Decompressor()
    {
        if (ResourceMgr::sInstance != NULL)
            ResourceMgr::sInstance->unregisterDecompressor(this);
    }

    virtual u8* tryDecompFromDevice(const ResourceMgr::LoadArg& loadArg, Resource* resource, u32* outSize, u32* outAllocSize, bool* outAllocated) = 0;

    FixedSafeString<32> mExt;
};

} // namespace sead

#endif // SEAD_DECOMPRESSOR_H_
