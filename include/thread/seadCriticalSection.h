#ifndef SEAD_CRITICAL_SECTION_H_
#define SEAD_CRITICAL_SECTION_H_

#ifdef cafe
#include <cafe.h>
#endif // cafe

#include <heap/seadDisposer.h>

namespace sead {

class CriticalSection : public IDisposer
{
public:
    CriticalSection();
    virtual ~CriticalSection();

    void lock();
    bool tryLock();
    void unlock();

#ifdef cafe
    OSMutex mCriticalSectionInner;
#else
    #error "Unknown platform"
#endif // cafe
};

} // namespace sead

#endif // SEAD_CRITICAL_SECTION_H_
