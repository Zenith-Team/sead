#include <filedevice/seadPath.h>
#include <prim/seadSafeString.hpp>

namespace sead {

bool Path::getDriveName(BufferedSafeString* driveName, const SafeString& path)
{
    driveName->trim(driveName->mBufferSize);

    s32 index = path.findIndex(":");
    if (index != -1)
        driveName->copy(path, index);

    return index != -1;
}

void Path::getPathExceptDrive(BufferedSafeString* pathNoDrive, const SafeString& path)
{
    pathNoDrive->trim(pathNoDrive->mBufferSize);

    s32 index = path.findIndex("://");
    if (index == -1)
        pathNoDrive->copyAt(0, path);

    else
        pathNoDrive->copyAt(0, path.getPart(index + 3));
}

} // namespace sead
