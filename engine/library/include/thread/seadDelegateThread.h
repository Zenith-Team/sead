#ifndef SEAD_DELEGATE_THREAD_H_
#define SEAD_DELEGATE_THREAD_H_

#include <thread/seadThread.h>

namespace sead {

template <typename T, typename U>
class IDelegate2;

class DelegateThread : public Thread
{
public:
    DelegateThread(const SafeString& name, IDelegate2<Thread*, MessageQueue::Element>* deleg, Heap* heap, s32 platformPriority = 0x10,
                   MessageQueue::BlockType blockType = MessageQueue::cBlock, MessageQueue::Element quitMsg = cDefaultQuitMsg,
                   s32 stackSize = cDefaultStackSize, s32 msgQueueSize = cDefaultMsgQueueSize);
    virtual ~DelegateThread();

protected:
    void calc_(MessageQueue::Element) override;

    IDelegate2<Thread*, MessageQueue::Element>* mDelegate;
};
#ifdef cafe
static_assert(sizeof(DelegateThread) == 0x94, "sead::DelegateThread size mismatch");
#endif // cafe

} // namespace sead

#endif // SEAD_DELEGATE_THREAD_H_
