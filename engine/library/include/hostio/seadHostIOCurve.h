#ifndef SEAD_HOST_IO_CURVE_H_
#define SEAD_HOST_IO_CURVE_H_

#include <math/seadVector.h>

namespace sead { namespace hostio {

class ICurve
{
public:
    virtual f32 interpolateToF32(f32 t) = 0;
    virtual Vector2f interpolateToVec2f(f32 t) = 0;
};

struct CurveDataInfo
{
    u8 curveType;
    u8 unitSize;
    u8 numBuf;
    u8 numUse;
};
static_assert(sizeof(CurveDataInfo) == 4);

template <typename T>
class Curve : public ICurve
{
public:
    Curve()
        : mData(nullptr)
    {
        mInfo.curveType = 0;
        mInfo.unitSize = sizeof(T);
        mInfo.numBuf = 0;
        mInfo.numUse = 0;
    }

    virtual f32 interpolateToF32(f32 t);
    virtual Vector2f interpolateToVec2f(f32 t);

private:
    T* mData;
    CurveDataInfo mInfo;
};
static_assert(sizeof(Curve<f32>) == 0xC);

template <typename T, u32 N>
struct CurveData
{
    u32 numUse;
    u32 curveType;
    T data[N];
};
static_assert(sizeof(CurveData<f32, 30>) == 0x80);

} } // namespace sead::hostio

#endif // SEAD_HOST_IO_CURVE_H_