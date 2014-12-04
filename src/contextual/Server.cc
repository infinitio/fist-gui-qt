#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <tchar.h>

#include <elle/finally.hh>
#include <elle/log.hh>

#include <contextual/Handler.hh>
#include <contextual/helpers.hh>
#include <contextual/HandlerFactory.hh>
#include <contextual/stdafx.h>

#include <strsafe.h>

HINSTANCE g_DllInstance = nullptr;

ELLE_LOG_COMPONENT("fist.contextual");

// {256EF94C-697D-4986-B99B-9D3B15D79B49}
DEFINE_GUID(
  CLSID_ContextMenu,
  0x256EF94C, 0x697D, 0x4986, 0xB9, 0x9B, 0x9D, 0x3B, 0x15, 0xD7, 0x9B, 0x49);

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllRegisterServer(void)
{
  ELLE_TRACE_SCOPE("register server (lock: %s)", g_DllLockCounter);
  return S_OK;
}

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllGetClassObject(REFCLSID rclsid,
                  REFIID riid,
                  void** ppv)
{
  ELLE_TRACE_SCOPE("get class object for id %s", rclsid);

  if (!IsEqualIID(rclsid, CLSID_ContextMenu))
    return CLASS_E_CLASSNOTAVAILABLE;

  // Check that ppv really points to a void*.
  if (IsBadWritePtr (ppv, sizeof(void*)))
    return E_POINTER;
  *ppv = nullptr;

  // Construct a new class factory object.
  fist::contextual::HandlerFactory* handler =
    new (std::nothrow) fist::contextual::HandlerFactory();
  if (handler == nullptr)
    return E_OUTOFMEMORY;

  handler->AddRef();
  elle::SafeFinally release{[&] { handler->Release(); }};
  return handler->QueryInterface(riid, ppv);
}

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllCanUnloadNow()
{
  ELLE_TRACE_SCOPE("check if the dll can unload (lock: %s)", g_DllLockCounter);

  return g_DllLockCounter > 0 ? S_FALSE : S_OK;
}

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllUnregisterServer()
{
  ELLE_TRACE_SCOPE("unregister server (lock: %s)", g_DllLockCounter);
  return S_OK;
}
