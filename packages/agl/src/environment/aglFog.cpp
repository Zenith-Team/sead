#include <environment/aglFog.h>

namespace agl { namespace env {

AGL_ENV_OBJ_REGIST_CLASS(Fog, "�t�H�O", 1);

Fog::Fog()
    : mStart    (1000.0f,                           "Start",    "�J�n",     this)
    , mEnd      (10000.0f,                          "End",      "�I��",     this)
    , mColor    (sead::Color4f::cWhite,             "Color",    "�J���[",   this)
    , mDirection(sead::Vector3f(0.0f, 0.0f, -1.0f),                         this)
{
}

ShaderMode Fog::drawDebug(const sead::Matrix34f& view_mtx, const sead::Matrix44f& proj_mtx, s32 view_index, ShaderMode mode) const
{
    return EnvObj::drawFog(view_index, *mStart, *mEnd, *mDirection, *mColor, mode);
}

} }
