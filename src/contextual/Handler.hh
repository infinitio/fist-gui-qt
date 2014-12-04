#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
// #include <ole2.h>
#include <commctrl.h>
// #include <shlwapi.h>
#include <shobjidl.h>

#include <vector>
#include <map>
#include <string>
#include <memory>

#include <elle/attribute.hh>

namespace fist
{
  namespace contextual
  {
    class Handler
      : public IContextMenu
      , public IShellExtInit
    {
    public:
      Handler();
      virtual
      ~Handler();

      enum class CommandId
      {
        get_a_link = 0,
        send,
        null,
      };

    public:
      /*---------.
      | IUnknown |
      \---------*/
      HRESULT WINAPI
      QueryInterface(REFIID riid,
                     void** ppvObject) override;

      ULONG WINAPI
      AddRef() override;

      ULONG WINAPI
      Release() override;

      /*--------------.
      | IShellExtInit |
      \--------------*/
      HRESULT WINAPI
      Initialize(PCIDLIST_ABSOLUTE pidlFolder,
                 IDataObject *pdtobj,
                 HKEY hkeyProgID) override;

      /*-------------.
      | IContextMenu |
      \-------------*/
      HRESULT WINAPI
      GetCommandString(UINT_PTR idCmd,
                       UINT uFlags,
                       UINT *pwReserved,
                       LPSTR pszName,
                       UINT cchMax) override;

      HRESULT WINAPI
      InvokeCommand(LPCMINVOKECOMMANDINFO pici) override;

      HRESULT WINAPI
      QueryContextMenu(HMENU hmenu,
                       UINT indexMenu,
                       UINT idCmdFirst,
                       UINT idCmdLast,
                       UINT uFlags) override;

      ELLE_ATTRIBUTE(ULONG, count);
    public:
      struct MenuItem
      {
        CommandId internal_id;
        PWSTR menu_text;
        PCSTR verb;
        PCSTR help;
        PCSTR name;
        PCWSTR verbw;
        PCWSTR helpw;
        PCWSTR namew;
      };

      typedef std::vector<MenuItem> Items;
      typedef std::map<CommandId, MenuItem> Commands;
      typedef std::vector<std::string> Files;
      ELLE_ATTRIBUTE_R(Commands, commands);
      ELLE_ATTRIBUTE_R(HANDLE, icon);
      ELLE_ATTRIBUTE_R(Files, files);

      CommandId
      find_verb(LPCSTR verb) const;

      CommandId
      find_verb(LPCWSTR wstr) const;
    };
  }
}

namespace std
{
  ostream&
  operator << (ostream& out,
               REFIID riid);

  ostream&
  operator << (ostream& out,
               fist::contextual::Handler::MenuItem const& item);

  ostream&
  operator << (ostream& out,
               fist::contextual::Handler::CommandId const& id);

  ostream&
  operator <<(ostream& out,
              MENUITEMINFO const& mii);
}
