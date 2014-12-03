#include <stddef.h>
#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#endif

#include <contextual/stdafx.h>
#include <contextual/helpers.hh>

namespace fist
{
  namespace windows
  {
    size_t
    to_wide(wchar_t* outIt,
            std::string const& s,
            CodePage::Specifier codepage)
    {
      return ::MultiByteToWideChar(
        codepage, 0, s.c_str(), static_cast<int>(s.size()),
        (wchar_t*)outIt, static_cast<int>(s.size() + 1));
    }

    size_t
    to_wide(wchar_t* outIt,
            std::wstring const& ws,
            CodePage::Specifier codepage)
    {
      size_t copied = ws.copy(outIt, ws.size());
      *(outIt + copied) = 0;
      return copied + 1;
    }

    std::wstring
    to_wide(std::string const& s,
            CodePage::Specifier codepage)
    {
      if (s.empty())
        return std::wstring();
      std::vector<wchar_t> wchars(s.size() + 1);
      int len = ::MultiByteToWideChar(
        codepage, 0, s.data(), static_cast<int>(s.size()),
        &wchars[0], static_cast<int>(wchars.size()) * sizeof(wchar_t));
      return std::wstring(wchars.begin(),  wchars.begin() + len);
    }

    std::wstring
    to_wide(char const* s,
            CodePage::Specifier codepage)
    {
      std::string str(s);
      return to_wide(str, codepage);
    }

    // to_narrow.
    size_t
    to_narrow(char* outIt,
              std::string const& s,
              CodePage::Specifier codepage)
    {
      size_t copied = s.copy(outIt, s.size());
      *(outIt + copied) = 0;
      return copied + 1;
    }

    size_t
    to_narrow(char* outIt,
              std::wstring const& s,
              CodePage::Specifier codepage)
    {
      return ::WideCharToMultiByte(codepage, 0, s.data(), static_cast<int>(s.size()),
                                   (char*)outIt, static_cast<int>(s.size() + 1) * sizeof(char),
                                   0, 0);
    }

    std::string
    to_narrow(std::wstring const& wstr,
              CodePage::Specifier codepage)
    {
      if (wstr.empty())
        return std::string();
      std::vector<char> vchars(wstr.size() * 2 + 1);
      int len = ::WideCharToMultiByte(
        codepage, 0, wstr.data(), static_cast<int>(wstr.size()),
        &vchars[0], static_cast<int>(vchars.size()) * sizeof(char),
        0, 0);
      return std::string(vchars.begin(),  vchars.begin() + len);
    }

    std::string
    to_narrow(wchar_t const* s,
              CodePage::Specifier codepage)
    {
      std::wstring wstr(s);
      return to_narrow(wstr, codepage);
    }
  }
}
