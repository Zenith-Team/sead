#ifndef SEAD_HEAPMGR_H_
#define SEAD_HEAPMGR_H_

#include <container/seadPtrArray.h>
#include <heap/seadArena.h>
#include <heap/seadHeap.h>
#include <thread/seadCriticalSection.h>

namespace sead {

class HeapMgr
{
public:
    HeapMgr();
    virtual ~HeapMgr() { }

    Heap* getCurrentHeap();
    Heap* findContainHeap(const void* ptr) const;

    static HeapMgr sInstance;
    static HeapMgr* sInstancePtr;

    static Arena sDefaultArena;
    static PtrArrayImpl sRootHeaps;
    static PtrArrayImpl sIndependentHeaps;
    static CriticalSection sHeapTreeLockCS;

    void* mAllocFailedCallback;  // IAllocFailedCallback* = IDelegate1<const AllocFailedCallbackArg*>*
};

} // namespace sead

#endif // SEAD_HEAPMGR_H_
