#ifndef FIST_GUI_QT_CONTEXTUAL_HELPERS_HH
# define FIST_GUI_QT_CONTEXTUAL_HELPERS_HH

# include <contextual/stdafx.h>

# include <string>

namespace fist
{
  namespace windows
  {
    LONG
    GetStringRegKey(HKEY hKey,
                    std::string const& name,
                    std::string& value,
                    std::string const& default_data = "");

    /*--------------------------------------------------.
    | Yet another string / widestring conversion helper |
    `--------------------------------------------------*/
    // Friendly names for the predefined Win32 code page specifiers.
    namespace CodePage {
      enum Specifier {
        ANSI    = CP_ACP,
        MacOS   = CP_MACCP,
        Windows = CP_OEMCP,  OEM = CP_OEMCP,
        Symbols = CP_SYMBOL, Symbol = CP_SYMBOL,
        UTF7    = CP_UTF7,
        UTF8    = CP_UTF8,
        US_Dos		= 437,
        Dos_Latin1	= 850,
        Dos_Latin2	= 852,
        Ansi_Latin1	= 1252
      };
    }

    // to_wide converts char-string, copies wstring (wrapper for
    // MultiByteToWideChar).
    std::wstring
    to_wide(std::string const& str,
            CodePage::Specifier codepage = CodePage::UTF8);

    std::wstring
    to_wide(char const* s,
            CodePage::Specifier codepage = CodePage::UTF8);

    size_t
    to_wide(wchar_t* outIt,
            std::string const& s,
            CodePage::Specifier codepage = CodePage::UTF8);

    size_t
    to_wide(wchar_t* outIt,
            std::wstring const& s,
            CodePage::Specifier codepage = CodePage::UTF8);

    //! to_narrow copies char-string, converts wstring (wrapper for
    // WideCharToMultiByte)
    std::string
    to_narrow(std::wstring const& wstr,
              CodePage::Specifier codepage = CodePage::UTF8);

    std::string
    to_narrow(wchar_t const* s,
              CodePage::Specifier codepage = CodePage::UTF8);

    size_t
    to_narrow(char* outIt,
              std::string const& s,
              CodePage::Specifier codepage = CodePage::UTF8);

    size_t
    to_narrow(char* outIt,
              std::wstring const& s,
              CodePage::Specifier codepage = CodePage::UTF8);

  }
}

#ifdef DEFINE_GUID
# undef DEFINE_GUID
#endif

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)    \
  extern "C" const GUID name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

#endif
