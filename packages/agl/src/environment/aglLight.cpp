#include <environment/aglLight.h>

namespace agl { namespace env {

AGL_ENV_OBJ_REGIST_CLASS(AmbientLight, "�A���r�G���g���C�g", 0);
AGL_ENV_OBJ_REGIST_CLASS(HemisphereLight, "�������C�g", 0);
AGL_ENV_OBJ_REGIST_CLASS(DirectionalLight, "�f�B���N�V���i�����C�g", 0);

AmbientLight::AmbientLight()
    : mColor    (sead::Color4f::cBlack, "Color",        "�A���r�G���g�J��?",                    this)
    , mIntensity(1.0f,                  "Intensity",    "�C���e���V�e�B",       "Min=0,Max=8",  this)
{
}

AmbientLight::~AmbientLight()
{
}

void AmbientLight::update()
{
}

ShaderMode AmbientLight::drawDebug(const sead::Matrix34f& view_mtx, const sead::Matrix44f& proj_mtx, s32 view_index, ShaderMode mode) const
{
    return mode;
}

HemisphereLight::HemisphereLight()
    : mSkyColor     (sead::Color4f(0.84f, 0.84f, 0.96f, 1.0f),  "SkyColor",     "�X�J�C�J���[",                     this)
    , mGroundColor  (sead::Color4f(0.47f, 0.35f, 0.25f, 1.0f),  "GroundColor",  "�O���E���h�J���[",                 this)
    , mIntensity    (1.0f,                                      "Intensity",    "�C���e���V�e�B",   "Min=0,Max=8",  this)
    , mDirection    (sead::Vector3f::ey,                                                                            this)
{
}

HemisphereLight::~HemisphereLight()
{
    mDiffuseDir.freeBuffer();
}

void HemisphereLight::initialize(s32 view_max, sead::Heap* heap)
{
    mDiffuseDir.allocBuffer(view_max, heap);
}

void HemisphereLight::updateView(const sead::Matrix34f& view_mtx, const sead::Matrix44f& proj_mtx, s32 view_index)
{
    mDiffuseDir[view_index].setRotate(view_mtx, *mDirection);
    mDiffuseDir[view_index].normalize();
}

ShaderMode HemisphereLight::drawDebug(const sead::Matrix34f& view_mtx, const sead::Matrix44f& proj_mtx, s32 view_index, ShaderMode mode) const
{
    return mode;
}

DirectionalLight::DirectionalLight()
    : mDiffuseColor (sead::Color4f::cWhite, "DiffuseColor",     "�f�B�t���[�Y�J���[",                           this)
    , mSpecularColor(sead::Color4f::cWhite, "SpecularColor",    "�X�y�L�����J���[",                             this)
    , mBacksideColor(sead::Color4f::cBlack, "BacksideColor",    "�����J���[",                                   this)
    , mIntensity    (1.0f,                  "Intensity",        "�C���e���V�e�B",       "Min=0,Max=8",          this)
    , mDirection    (-sead::Vector3f::ones,                                                                     this)
{
    mDirection->normalize();
    syncFromDirection_();
}

DirectionalLight::~DirectionalLight()
{
    mDiffuseDir.freeBuffer();
}

void DirectionalLight::syncFromDirection_()
{
    sead::Vector3f dir;
    if (dir.setNormalize(*mDirection) > 0.0f)
    {
        sead::Vector2f norm_zx(-dir.z, -dir.x);
        if (norm_zx.normalize() > 0.0f)
            _1dc.x = sead::Mathf::atan2(norm_zx.y, norm_zx.x);

        f32 norm_y = sead::Mathf::clamp2(-1.0f, -dir.y, 1.0f);
        _1dc.y = sead::Mathf::asin(norm_y);
    }
}

void DirectionalLight::initialize(s32 view_max, sead::Heap* heap)
{
    mDiffuseDir.allocBuffer(view_max, heap);
}

void DirectionalLight::updateView(const sead::Matrix34f& view_mtx, const sead::Matrix44f& proj_mtx, s32 view_index)
{
    mDiffuseDir[view_index].setRotate(view_mtx, *mDirection);
    mDiffuseDir[view_index].normalize();
}

ShaderMode DirectionalLight::drawDebug(const sead::Matrix34f& view_mtx, const sead::Matrix44f& proj_mtx, s32 view_index, ShaderMode mode) const
{
    // TODO
    return mode;
}

void DirectionalLight::callbackLoadData()
{
    syncFromDirection_();
}

} }
