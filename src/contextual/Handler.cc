#include <winsock2.h>
#include <windows.h>
#include <shlobj.h>
#include <tchar.h>
#include <strsafe.h>

#include <ios>

#include <elle/log.hh>
#include <elle/system/Process.hh>
#include <elle/container/map.hh>
#include <elle/finally.hh>
#include <elle/container/vector.hh>

#include <contextual/Handler.hh>
#include <contextual/helpers.hh>
#include <contextual/resources.hh>
#include <contextual/stdafx.h>

#include <contextual/bitmap.h>

ELLE_LOG_COMPONENT("fist.contextual.Handler");

UINT g_DllLockCounter = 0;

#include <uxtheme.h>

namespace fist
{
  namespace contextual
  {
    static
    std::string const&
    infinit_install_directory()
    {
      static std::unique_ptr<std::string> install_dir;
      if (install_dir == nullptr)
      {
        install_dir.reset(new std::string);
        HKEY hKey;
        if (RegOpenKeyExA(
              HKEY_LOCAL_MACHINE,
              "SOFTWARE\\Infinit.io\\Infinit",
              0,
              KEY_QUERY_VALUE ,
              &hKey) == ERROR_SUCCESS)
        fist::windows::GetStringRegKey(hKey, "Install_Dir", *install_dir, "");
        else if (RegOpenKeyExA(
                   HKEY_LOCAL_MACHINE,
                   "SOFTWARE\\Infinit.io\\Infinit",
                   0,
                   KEY_QUERY_VALUE | KEY_WOW64_32KEY,
                   &hKey) == ERROR_SUCCESS)
          fist::windows::GetStringRegKey(hKey, "Install_Dir", *install_dir, "");
        else if (RegOpenKeyExA(
                   HKEY_LOCAL_MACHINE,
                   "SOFTWARE\\Infinit.io\\Infinit",
                   0,
                   KEY_QUERY_VALUE | KEY_WOW64_64KEY,
                   &hKey) == ERROR_SUCCESS)
          fist::windows::GetStringRegKey(hKey, "Install_Dir", *install_dir, "");
      }
      return *install_dir;
    }

    Handler::Handler()
      : _count(1)
      , _commands(
        {
          {
            CommandId::get_a_link,
            {
              CommandId::get_a_link,
              L"&Copy Infinit link",
              "&Copy Infinit link",
              "GetALink",
              "Copy Infinit link",
              "GetALink",
              L"GetALink",
              L"Copy Infinit link",
              L"GetALink",
            },
          },
          {
            CommandId::send,
            {
              CommandId::send,
              L"&Send with Infinit",
              "&Send with Infinit",
              "SendWithInfinit",
              "Send with Infinit",
              "SendWithInfinit",
              L"SendWithInfinit",
              L"Send with Infinit",
              L"SendWithInfinit",
            }
          },
      })
      , _icon(LoadImage(g_DllInstance, MAKEINTRESOURCE(100),
                        IMAGE_BITMAP, 16, 16, LR_SHARED | LR_LOADTRANSPARENT))
      , _files()
    {
      ELLE_TRACE_SCOPE("%s: creation", *this);
      InterlockedIncrement(&g_DllLockCounter);

      // The default icon has transparency issue. The bitmap manipulation code
      // has been taken from NotePad++,
      HICON hicon = (HICON) LoadImage(
        g_DllInstance, MAKEINTRESOURCE(101),
        IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
      elle::SafeFinally delete_hicon([&] { DestroyIcon(hicon); });
      HBITMAP icon = NULL;
      if (InitTheming())
      {
        icon = IconToBitmapPARGB32(hicon, 16, 16);
        DeinitTheming();
      }
      if (icon)
      {
        DeleteObject(this->_icon);
        this->_icon = icon;
      }
    }

    Handler::~Handler()
    {
      ELLE_TRACE_SCOPE("%s: destruction", *this);
      InterlockedDecrement(&g_DllLockCounter);

      if (this->_icon)
      {
        DeleteObject(this->_icon);
        this->_icon = nullptr;
      }

    }

    // ContextualMenuHandler _Module;
    HRESULT
    Handler::QueryInterface(REFIID riid,
                            void** ppvObject)
    {
      ELLE_TRACE_SCOPE("%s: query as %s", *this, riid);
      if (IsBadWritePtr (ppvObject, sizeof(void*)))
        ELLE_WARN("bad pointer")
          return E_POINTER;
      if (ppvObject == NULL)
        ELLE_WARN("invalid pointer")
          return E_POINTER;
      HRESULT hrRet = S_OK;
      *ppvObject = NULL;
      if (riid == IID_IContextMenu)
        ELLE_DEBUG("as a IContextMenu")
          *ppvObject = static_cast<IContextMenu*>(this);
      else if (riid == IID_IShellExtInit)
        ELLE_DEBUG("as a IShellExtInit")
          *ppvObject = static_cast<IShellExtInit*>(this);
      else if (riid == IID_IUnknown)
        ELLE_DEBUG("as a IUnknown")
          *ppvObject = static_cast<IContextMenu*>(this);
      else
        ELLE_DEBUG("no interface")
          hrRet = E_NOINTERFACE;
      if (hrRet == S_OK)
        this->AddRef();
      return hrRet;
    }

    ULONG
    Handler::AddRef()
    {
      ELLE_TRACE_SCOPE("%s: add ref (currently %s)", *this, this->_count);
      return InterlockedIncrement(&this->_count);
    }

    ULONG
    Handler::Release()
    {
      ELLE_TRACE_SCOPE("%s: release ref (currently %s)", *this, this->_count);
      ULONG ref = InterlockedDecrement(&this->_count);
      if (ref == 0)
        delete this;
      return ref;
    }

    HRESULT
    Handler::Initialize(LPCITEMIDLIST pidlFolder,
                        LPDATAOBJECT dataObject,
                        HKEY /* hkeyProgID */)
    {
      ELLE_TRACE_SCOPE("%s: Initialize", *this);

      if (NULL == dataObject)
        ELLE_WARN("invalid argument")
          return E_INVALIDARG;

      HRESULT hr = E_FAIL;
      FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
      STGMEDIUM stm;

      // The pDataObj pointer contains the objects being acted upon. In this
      // example, we get an HDROP handle for enumerating the selected files and
      // folders.
      if (SUCCEEDED(dataObject->GetData(&fe, &stm)))
      {
        ELLE_DEBUG("get data");
        // Get an HDROP handle.
        HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (hDrop != NULL)
        {
          this->_files.clear();
          // Determine how many files are involved in this operation. This
          // code sample displays the custom context menu item when only
          // one file is selected.
          UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
          if (nFiles == 0)
            return E_FAIL;

          wchar_t file[MAX_PATH];
          for (UINT i = 0; i < nFiles; ++i)
          {
            if (DragQueryFileW(hDrop, i, file, MAX_PATH) != 0)
            {
              this->_files.push_back(
                elle::sprintf(
                  "\"%s\"",
                  fist::windows::to_narrow(
                    std::wstring(file),
                    fist::windows::CodePage::Specifier::UTF8)));
            }
          }

          if (!this->_files.empty())
            hr = S_OK;
          ELLE_TRACE("%s files", this->_files.size());

          ELLE_DEBUG("unlock stm")
            GlobalUnlock(stm.hGlobal);
        }

        ELLE_DEBUG("release medium")
          ReleaseStgMedium(&stm);
      }

      // If any value other than S_OK is returned from the method, the context
      // menu item is not displayed.
      return hr;
    }

    HRESULT
    Handler::GetCommandString(UINT_PTR idCommand,
                              UINT uFlags,
                              UINT *pwReserved,
                              LPSTR pszName,
                              UINT cchMax)
    {
      ELLE_TRACE_SCOPE("%s: get command string", *this);
      HRESULT hr = E_INVALIDARG;

      if (uFlags == GCS_VALIDATEA || uFlags == GCS_VALIDATEW)
      {
        if (idCommand < 0 || idCommand >= this->_commands.size())
        {
          ELLE_WARN("validation failed");
          return S_FALSE;
        }
        else
        {
          ELLE_DEBUG("validation succeeded");
          return S_OK;
        }
      }
      if (idCommand < 0 || idCommand >= this->_commands.size())
        return E_FAIL;
      auto command = this->_commands.at(static_cast<CommandId>(idCommand));
      ELLE_TRACE("%s", command)
        if (uFlags == GCS_HELPTEXTA)
        {
          ELLE_DEBUG("ask for help string");
          ::strcpy(pszName, command.help);
          hr = NO_ERROR;
        }
        else if (uFlags == GCS_HELPTEXTW)
        {
          ELLE_DEBUG("ask for help wstring");
          hr = StringCchCopyW(reinterpret_cast<PWSTR>(pszName), cchMax, command.helpw);
        }
        else if (uFlags == GCS_VERBA)
        {
          ELLE_DEBUG("ask for verb string");
          ::strcpy(pszName, command.verb);
          hr = NO_ERROR;
        }
        else if (uFlags == GCS_VERBW)
        {
          ELLE_DEBUG("ask for verb wstring");
          hr = StringCchCopyW(reinterpret_cast<PWSTR>(pszName), cchMax, command.verbw);
        }
      return hr;
    }

    Handler::CommandId
    Handler::find_verb(LPCSTR str) const
    {
      ELLE_DEBUG_SCOPE("%s: find verb %s", *this, str);
      if (str == nullptr)
        return CommandId::null;
      for (auto& entry: this->_commands)
      {
        if (::strcmp(str, entry.second.verb) == 0)
          return entry.second.internal_id;
      }
      return CommandId::null;
    }

    Handler::CommandId
    Handler::find_verb(LPCWSTR wstr) const
    {
      ELLE_DEBUG_SCOPE("%s: find verb", *this);
      if (wstr == nullptr)
        return CommandId::null;
      for (auto& entry: this->_commands)
      {
        if (::wcscmp(wstr, entry.second.verbw) == 0)
          return entry.second.internal_id;
      }
      return CommandId::null;
    }

    HRESULT
    Handler::InvokeCommand(LPCMINVOKECOMMANDINFO command_info)
    {
      ELLE_TRACE_SCOPE("%s: invoke command", *this);

      CommandId commandId = CommandId::null;

      if (command_info->cbSize == sizeof(CMINVOKECOMMANDINFOEX) &&
          (command_info->fMask & CMIC_MASK_UNICODE) != 0)
      {
        LPCMINVOKECOMMANDINFOEX command_infoEx =
          (LPCMINVOKECOMMANDINFOEX) command_info;
        if (HIWORD(command_infoEx->lpVerbW) == 0)
          commandId = static_cast<CommandId>(LOWORD(command_info->lpVerb));
        else
          commandId = this->find_verb(command_infoEx->lpVerbW);
      }
      else
      {
        if (HIWORD(command_info->lpVerb) == 0)
          commandId = static_cast<CommandId>(LOWORD(command_info->lpVerb));
        else
          commandId = this->find_verb(command_info->lpVerb);
      }
      ELLE_TRACE_SCOPE("-command id: %s", commandId);

      if (commandId == CommandId::null)
        return E_FAIL;

      std::string const& install_dir = infinit_install_directory();
      {
        if (install_dir.empty())
          return E_FAIL;
      }
      std::string exe = install_dir + "\\Infinit.exe";
      ELLE_DEBUG("executable path: %s", exe);
      auto run = [&] (std::vector<std::string> args)
      {
        ELLE_TRACE("run %s", args);
        args.insert(args.begin(), exe);
        elle::system::Process p{args};
      };
      switch(commandId)
      {
        case CommandId::send:
          this->_files.insert(this->_files.begin(), "--add");
          run(this->_files);
          break;
        case CommandId::get_a_link:
          this->_files.insert(this->_files.begin(), "--link");
          run(this->_files);
          break;
        case CommandId::null:
          ELLE_ERR("invalid command");
          break;
      }
      ELLE_DEBUG("clear files")
        this->_files.clear();

      return S_OK;
    }

    HRESULT
    Handler::QueryContextMenu(HMENU hmenu,
                              UINT indexMenu,
                              UINT idCmdFirst,
                              UINT idCmdLast,
                              UINT uFlags)
    {
      ELLE_TRACE_SCOPE("%s: query context menu", *this);
      if (CMF_DEFAULTONLY & uFlags)
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
      // Stolen from the internet...
      if ((uFlags & 0x000F) != CMF_NORMAL &&
          (uFlags & CMF_VERBSONLY) == 0 && (uFlags & CMF_EXPLORE) == 0)
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, indexMenu);
      if (infinit_install_directory().empty())
        return E_FAIL;
      for (auto command_pair: this->_commands)
      {
        auto& command = command_pair.second;
        UINT index = static_cast<UINT>(command.internal_id);
        ELLE_TRACE("add %s to the contextual menu", command);
        MENUITEMINFO mii = { sizeof(mii) };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_BITMAP | MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE;
        mii.wID = idCmdFirst + index;
        mii.fType = MFT_STRING;
        mii.dwTypeData = command.menu_text;
        mii.fState = MFS_ENABLED;
        mii.hbmpItem = static_cast<HBITMAP>(this->_icon);

        ELLE_DEBUG("insert %s", mii)
          if (!InsertMenuItem(hmenu, indexMenu + index, TRUE, &mii))
            ELLE_ERR("fail at inserting command on the contextual")
              return HRESULT_FROM_WIN32(GetLastError());
      }

      // Add a separator.
      {
        MENUITEMINFO sep = { sizeof(sep) };
        sep.fMask = MIIM_TYPE;
        sep.fType = MFT_SEPARATOR;
        if (!InsertMenuItem(hmenu, indexMenu + this->_commands.size(), TRUE, &sep))
          return HRESULT_FROM_WIN32(GetLastError());
      }

      // Return an HRESULT value with the severity set to SEVERITY_SUCCESS.
      // Set the code value to the offset of the largest command identifier
      // that was assigned, plus one (1).
      return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(this->_commands.size()));
    }
  }
}

namespace std
{
  ostream&
  operator << (ostream& out,
               REFIID riid)
  {
    return out
      << hex
      << "{"
      << riid.Data1 << "-"
      << riid.Data2 << "-"
      << riid.Data3 << "-"
      << "..."
      << "}";
  }

  ostream&
  operator << (ostream& out,
               fist::contextual::Handler::MenuItem const& item)
  {
#define PINN(field) #field ": " << ((item.field != nullptr) ? string(item.field) : string{})
    return out << "Item("
               << PINN(name) << ", "
               << PINN(verb) << ", "
               << PINN(help)
               << ")";
  }

  ostream&
  operator << (ostream& out,
               fist::contextual::Handler::CommandId const& id)
  {
    switch (id)
    {
      case fist::contextual::Handler::CommandId::send:
        out << "send";
        break;
      case fist::contextual::Handler::CommandId::get_a_link:
        out << "get a link";
        break;
      case fist::contextual::Handler::CommandId::null:
        out << "null";
        break;
    }
    return out;
  }

  ostream&
  operator <<(ostream& out,
              MENUITEMINFO const& mii)
  {
    return out << "MenuInfo("
               << "index: " << mii.wID << ", "
               << "data: " << mii.dwTypeData
               << ")";
  }
}
