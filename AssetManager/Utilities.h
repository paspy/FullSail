#ifndef Utilities_h__
#define Utilities_h__
#include <string>
#include <codecvt>

namespace AssetUtilities {
    static std::string ws2s ( const std::wstring& wstr ) {
        typedef std::codecvt_utf8_utf16<wchar_t> convert_type;
        std::wstring_convert<convert_type> converter;
        return converter.to_bytes ( wstr );
    }

    static std::wstring s2ws( const std::string& str ) {
        typedef std::codecvt_utf8_utf16<wchar_t> convert_type;
        std::wstring_convert<convert_type> converter;
        return converter.from_bytes( str );
    }
}

#endif // Utilities_h__
