#include <prim/seadSafeString.h>
#include <prim/seadStringUtil.h>

namespace {

static const char16 cEmptyStringChar16[1] = L"";

} // namespace {no-name}

namespace sead {

template <>
const char SafeStringBase<char>::cNullChar = '\0';

template <>
const char SafeStringBase<char>::cLineBreakChar = '\n';

template <>
const SafeStringBase<char> SafeStringBase<char>::cEmptyString("");

template <>
const char16 SafeStringBase<char16>::cNullChar = 0;

template <>
const char16 SafeStringBase<char16>::cLineBreakChar = static_cast<char16>('\n');

template <>
const SafeStringBase<char16> SafeStringBase<char16>::cEmptyString(cEmptyStringChar16);

template <>
s32 BufferedSafeStringBase<char>::formatImpl_(char* s, s32 n, const char* formatStr, va_list args)
{
    return StringUtil::vsnprintf(s, n, formatStr, args);
}

template <>
s32 BufferedSafeStringBase<char16>::formatImpl_(char16* s, s32 n, const char16* formatStr, va_list args)
{
    return StringUtil::vsnw16printf(s, n, formatStr, args);
}

template <>
s32 BufferedSafeStringBase<char>::formatV(const char* formatStr, va_list args)
{
    char* mutableString = getMutableStringTop_();
    return formatImpl_(mutableString, mBufferSize, formatStr, args);
}

template <>
s32 BufferedSafeStringBase<char16>::formatV(const char16* formatStr, va_list args)
{
    char16* mutableString = getMutableStringTop_();
    return formatImpl_(mutableString, mBufferSize, formatStr, args);
}

template <>
s32 BufferedSafeStringBase<char>::format(const char* formatStr, ...)
{
    va_list args;
    va_start(args, formatStr);
    s32 ret = formatV(formatStr, args);
    va_end(args);

    return ret;
}

template <>
s32 BufferedSafeStringBase<char16>::format(const char16* formatStr, ...)
{
    va_list args;
    va_start(args, formatStr);
    s32 ret = formatV(formatStr, args);
    va_end(args);

    return ret;
}

} // namespace sead
