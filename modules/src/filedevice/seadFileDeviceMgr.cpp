#ifdef cafe
#include <cafe.h>
#include <nn/save.h>
#endif // cafe

#include <heap/seadDisposer.h>
#include <filedevice/seadFileDeviceMgr.h>
#include <filedevice/seadPath.h>
#include <heap/seadDisposer.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <prim/seadSafeString.h>

namespace sead {

SEAD_SINGLETON_DISPOSER_IMPL(FileDeviceMgr)

FileDeviceMgr::FileDeviceMgr()
    : mDeviceList()
    , mMainFileDevice(NULL)
    , mDefaultFileDevice(NULL)
{
    if (!HeapMgr::isInitialized())
    {
        //SEAD_ASSERT_MSG(false, "FileDeviceMgr need HeapMgr");
        return;
    }

    Heap* heap = HeapMgr::instance()->findContainHeap(this);

#ifdef cafe
    FSInit();
    FSAddClient(&mFSClient, FS_RET_NO_ERROR);

    FSStateChangeParams changeParams = {
        .userCallback = stateChangeCallback_,
        .userContext  = NULL,
        .ioMsgQueue   = NULL
    };

    FSSetStateChangeNotification(&mFSClient, &changeParams);
    SAVEInit();

    //mountHostFileIO();

    mFSSDMountPath[0] = 0;
    mFSSDMountCount = 0;
#else
    #error "Unknown platform"
#endif // cafe

    mMainFileDevice = new(heap, 4) MainFileDevice(heap);
    mount(mMainFileDevice);

    mDefaultFileDevice = mMainFileDevice;
}

FileDeviceMgr::~FileDeviceMgr()
{
    if (mMainFileDevice != NULL)
    {
        delete mMainFileDevice;
        mMainFileDevice = NULL;
    }

#ifdef cafe
    //unmountHostFileIO();
    FSDelClient(&mFSClient, FS_RET_NO_ERROR);
    SAVEShutdown();
    FSShutdown();
#else
    #error "Unknown platform"
#endif // cafe
}

void FileDeviceMgr::traceFilePath(const SafeString& path) const
{
    FixedSafeString<cNoDrivePathBufferSize> no_drive_path;
    FileDevice* device = findDeviceFromPath(path, &no_drive_path);

    if (device == NULL)
    {
        //SEAD_WARNING(false, "FileDevice not found: %s\n", path.cstr());
        return;
    }

    device->traceFilePath(no_drive_path);
}

void FileDeviceMgr::traceDirectoryPath(const SafeString& path) const
{
    FixedSafeString<cNoDrivePathBufferSize> no_drive_path;
    FileDevice* device = findDeviceFromPath(path, &no_drive_path);

    if (device == NULL)
    {
        //SEAD_WARNING(false, "FileDevice not found: %s\n", path.cstr());
        return;
    }

    device->traceDirectoryPath(no_drive_path);
}

void FileDeviceMgr::resolveFilePath(BufferedSafeString* out, const SafeString& path) const
{
    FixedSafeString<cNoDrivePathBufferSize> no_drive_path;
    FileDevice* device = findDeviceFromPath(path, &no_drive_path);

    if (device == NULL)
    {
        //SEAD_WARNING(false, "FileDevice not found: %s\n", path.cstr());
        return;
    }

    device->resolveFilePath(out, no_drive_path);
}

void FileDeviceMgr::resolveDirectoryPath(BufferedSafeString* out, const SafeString& path) const
{
    FixedSafeString<cNoDrivePathBufferSize> no_drive_path;
    FileDevice* device = findDeviceFromPath(path, &no_drive_path);

    if (device == NULL)
    {
        //SEAD_WARNING(false, "FileDevice not found: %s\n", path.cstr());
        return;
    }

    device->resolveDirectoryPath(out, no_drive_path);
}

void FileDeviceMgr::mount(FileDevice* device, const SafeString& drive_name)
{
    if (!drive_name.isEqual(SafeString::cEmptyString))
        device->mDriveName.copy(drive_name);

    mDeviceList.pushBack(device);
}

void FileDeviceMgr::unmount(FileDevice* device)
{
    if (device->mList != NULL)
        mDeviceList.erase(device);

    if (device == mDefaultFileDevice)
        mDefaultFileDevice = NULL;
}

FileDevice*
FileDeviceMgr::findDeviceFromPath(
    const SafeString& path, BufferedSafeString* no_drive_path
) const
{
    FixedSafeString<FileDevice::cDriveNameBufferSize> drive;

    bool success = Path::getDriveName(&drive, path);
    FileDevice* device;

    if (success)
    {
        device = findDevice(drive);
        //SEAD_ASSERT(device);
    }
    else
    {
        if (mDefaultFileDevice == NULL)
        {
            //SEAD_ASSERT_MSG(false, "drive name not found and default file device is null");
            return NULL;
        }
        device = mDefaultFileDevice;
    }

    if (no_drive_path != NULL)
        Path::getPathExceptDrive(no_drive_path, path);

    return device;
}

FileDevice*
FileDeviceMgr::findDevice(const SafeString& drive) const
{
    for (FileDeviceMgr::DeviceList::iterator it = mDeviceList.begin(); it != mDeviceList.end(); ++it)
        if ((*it)->mDriveName.isEqual(drive))
            return (*it);

    return NULL;
}

FileDevice* FileDeviceMgr::tryOpen(FileHandle* handle, const SafeString& filename, FileDevice::FileOpenFlag flag, u32 div_num)
{
    FixedSafeString<cNoDrivePathBufferSize> no_drive_path;
    FileDevice* device = findDeviceFromPath(filename, &no_drive_path);

    if (device == NULL)
        return NULL;

    return device->tryOpen(handle, no_drive_path, flag, div_num);
}

u8* FileDeviceMgr::tryLoad(FileDevice::LoadArg& arg)
{
    //SEAD_ASSERT_MSG(!arg.path.isEqual(SafeString::cEmptyString), "path is null");

    FixedSafeString<cNoDrivePathBufferSize> no_drive_path;
    FileDevice* device = findDeviceFromPath(arg.path, &no_drive_path);

    if (device == NULL)
        return NULL;

    FileDevice::LoadArg arg_(arg);
    arg_.path = no_drive_path.cstr();

    u8* ret = device->tryLoad(arg_);

    arg.read_size = arg_.read_size;
    arg.roundup_size = arg_.roundup_size;
    arg.need_unload = arg_.need_unload;

    return ret;
}

#ifdef cafe
void
FileDeviceMgr::stateChangeCallback_(
    FSClient* client, FSVolumeState state, void* context
)
{
    FSError error = FSGetLastError(client);
    //SEAD_WARNING(false, "Volume state of client 0x%08x changed to %d, last error is %d\n", (uintptr_t)client, (s32)state, error);
}
#endif // cafe

}
