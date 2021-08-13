#ifndef SEAD_COLOR_H_
#define SEAD_COLOR_H_

#include <basis/seadTypes.h>

namespace sead {

class Color4f
{
public:
    Color4f(f32 r, f32 g, f32 b, f32 a)
        : r(r)
        , g(g)
        , b(b)
        , a(a)
    {
    }

    f32 r;
    f32 g;
    f32 b;
    f32 a;

    static const Color4f cWhite;
    static const Color4f cRed;
    static const Color4f cGreen;
    static const Color4f cBlue;
};

}  // namespace sead

#endif // #define SEAD_COLOR_H_
