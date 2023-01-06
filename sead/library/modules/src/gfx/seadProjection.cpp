#include <gfx/seadProjection.h>

namespace sead {

Projection::Projection()
    : mDirty(true)
    , mDeviceDirty(true)
    , mMatrix()
    , mDeviceMatrix()
    , mDevicePosture(Graphics::getDefaultDevicePosture())
    , mDeviceZScale(Graphics::getDefaultDeviceZScale())
    , mDeviceZOffset(Graphics::getDefaultDeviceZOffset())
{
}

Projection::~Projection()
{
}

void Projection::updateMatrixImpl_() const
{
    if (mDirty)
    {
        doUpdateMatrix(const_cast<Matrix44f*>(&mMatrix));
        mDirty = false;
        mDeviceDirty = true;
    }

    if (mDeviceDirty)
    {
        doUpdateDeviceMatrix(const_cast<Matrix44f*>(&mDeviceMatrix), mMatrix, mDevicePosture);
        mDeviceDirty = false;
    }
}

const Matrix44f& Projection::getDeviceProjectionMatrix() const
{
    updateMatrixImpl_();
    return mDeviceMatrix;
}

namespace {

void swapMatrixXY(Matrix44f* tgt)
{
    Vector4f x;
    Vector4f y;
    tgt->getRow(x, 0);
    tgt->getRow(y, 1);
    tgt->setRow(0, y);
    tgt->setRow(1, x);
}

}

void Projection::doUpdateDeviceMatrix(Matrix44f* dst, const Matrix44f& src, Graphics::DevicePosture pose) const
{
    *dst = src;

    switch (pose)
    {
    case Graphics::cDevicePosture_Same:
        break;
    case Graphics::cDevicePosture_RotateLeft:
        (*dst)(1, 0) *= -1;
        (*dst)(1, 1) *= -1;
        (*dst)(1, 2) *= -1;
        (*dst)(1, 3) *= -1;
        swapMatrixXY(dst);
        break;
    case Graphics::cDevicePosture_RotateRight:
        (*dst)(0, 0) *= -1;
        (*dst)(0, 1) *= -1;
        (*dst)(0, 2) *= -1;
        (*dst)(0, 3) *= -1;
        swapMatrixXY(dst);
        break;
    case Graphics::cDevicePosture_RotateHalfAround:
        (*dst)(0, 0) *= -1;
        (*dst)(0, 1) *= -1;
        (*dst)(0, 2) *= -1;
        (*dst)(0, 3) *= -1;
        (*dst)(1, 0) *= -1;
        (*dst)(1, 1) *= -1;
        (*dst)(1, 2) *= -1;
        (*dst)(1, 3) *= -1;
        break;
    case Graphics::cDevicePosture_FlipX:
        (*dst)(0, 0) *= -1;
        (*dst)(0, 1) *= -1;
        (*dst)(0, 2) *= -1;
        (*dst)(0, 3) *= -1;
        break;
    case Graphics::cDevicePosture_FlipY:
        (*dst)(1, 0) *= -1;
        (*dst)(1, 1) *= -1;
        (*dst)(1, 2) *= -1;
        (*dst)(1, 3) *= -1;
        break;
    default:
        // SEAD_ASSERTMSG(false, "Invalid DevicePosture(%d).", s32(pose));
    }

    (*dst)(2, 0) = (*dst)(2, 0) * mDeviceZScale;
    (*dst)(2, 1) = (*dst)(2, 1) * mDeviceZScale;

    (*dst)(2, 2) = ((*dst)(2, 2) + (*dst)(3, 2) * mDeviceZOffset) * mDeviceZScale;
    (*dst)(2, 3) = (*dst)(2, 3) * mDeviceZScale + (*dst)(3, 3) * mDeviceZOffset;
}

PerspectiveProjection::PerspectiveProjection(f32 near, f32 far, f32 fovy_rad, f32 aspect)
    : Projection()
    , mNear(near)
    , mFar(far)
    , mAspect(aspect)
    , mOffset(Vector2f::zero)
{
    setFovy_(fovy_rad);
    setDirty();
}

PerspectiveProjection::~PerspectiveProjection()
{
}

void PerspectiveProjection::setFovy_(f32 fovy)
{
    mAngle = fovy;

    fovy *= 0.5f;
    mFovySin = Mathf::sin(fovy);
    mFovyCos = Mathf::cos(fovy);
    mFovyTan = Mathf::tan(fovy);

    setDirty();
}

f32 PerspectiveProjection::getTop() const
{
    f32 clip_height = calcNearClipHeight_();
    f32 center_y = mOffset.y * clip_height;
    return clip_height * 0.5f + center_y;
}

f32 PerspectiveProjection::getBottom() const
{
    f32 clip_height = calcNearClipHeight_();
    f32 center_y = mOffset.y * clip_height;
    return -clip_height * 0.5f + center_y;
}

f32 PerspectiveProjection::getLeft() const
{
    f32 clip_width = calcNearClipWidth_();
    f32 center_x = mOffset.x * clip_width;
    return -clip_width * 0.5f + center_x;
}

f32 PerspectiveProjection::getRight() const
{
    f32 clip_width = calcNearClipWidth_();
    f32 center_x = mOffset.x * clip_width;
    return clip_width * 0.5f + center_x;
}

void PerspectiveProjection::doUpdateMatrix(Matrix44f* dst) const
{
    f32 clip_height = calcNearClipHeight_();
    f32 clip_width  = calcNearClipWidth_();

    f32 center_x = clip_width * mOffset.x;
    f32 center_y = clip_height * mOffset.y;

    clip_height *= 0.5f;
    clip_width  *= 0.5f;

    f32 top    =  clip_height + center_y;
    f32 bottom = -clip_height + center_y;

    f32 left   = -clip_width  + center_x;
    f32 right  =  clip_width  + center_x;

    f32 inv_size = 1.0f / (right - left);

    (*dst)(0, 0) = mNear * 2 * inv_size;
    (*dst)(0, 1) = 0.0f;
    (*dst)(0, 2) = (right + left) * inv_size;
    (*dst)(0, 3) = 0.0f;

    inv_size = 1.0f / (top - bottom);

    (*dst)(1, 0) = 0.0f;
    (*dst)(1, 1) = mNear * 2 * inv_size;
    (*dst)(1, 2) = (top + bottom) * inv_size;
    (*dst)(1, 3) = 0.0f;

    inv_size = 1.0f / (mFar - mNear);

    (*dst)(2, 0) = 0.0f;
    (*dst)(2, 1) = 0.0f;
    (*dst)(2, 2) = -(mFar + mNear) * inv_size;
    (*dst)(2, 3) = -(mFar * 2 * mNear) * inv_size;

    (*dst)(3, 0) = 0.0f;
    (*dst)(3, 1) = 0.0f;
    (*dst)(3, 2) = -1.0f;
    (*dst)(3, 3) = 0.0f;
}

void PerspectiveProjection::doScreenPosToCameraPosTo(Vector3f* dst, const Vector3f& screen_pos) const
{
    dst->set(0.0f, 0.0f, -mNear);

    dst->y = (calcNearClipHeight_() / 2) * (screen_pos.y + mOffset.y * 2);
    dst->x = (calcNearClipWidth_() / 2) * (screen_pos.x + mOffset.x * 2);
}

}
