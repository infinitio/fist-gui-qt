#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <tchar.h>

#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/os/environ.hh>
#include <elle/log/TextLogger.hh>

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

static
void
initialize_logger()
{
#ifndef ELLE_LOG_DISABLE
  if (elle::os::inenv("INFINIT_LOG_FILE"))
  {
    auto* stream = new std::ofstream{elle::os::getenv("INFINIT_LOG_FILE"), std::fstream::app | std::fstream::out};
    std::unique_ptr<elle::log::Logger> f(new elle::log::TextLogger(*stream, "TRACE", true, true, true, true, false));
    elle::log::logger(std::move(f));
  }
#endif
}

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllMain(HMODULE hModule,
        DWORD dwReason,
        LPVOID lpReserved);

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllMain(HMODULE hModule,
          DWORD dwReason,
          LPVOID lpReserved)
{
  switch (dwReason)
  {
    case DLL_PROCESS_ATTACH:
      // Hold the instance of this DLL module, we will use it to get the
      // path of the DLL to register the component.
      g_DllInstance = hModule;
      DisableThreadLibraryCalls(hModule);
      break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
     break;
  }
  return TRUE;
}

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllRegisterServer(void);

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllRegisterServer(void)
{
  initialize_logger();
  ELLE_TRACE_SCOPE("register server (lock: %s)", g_DllLockCounter);
  return S_OK;
}

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllGetClassObject(REFCLSID rclsid,
                  REFIID riid,
                  void** ppv);

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllGetClassObject(REFCLSID rclsid,
                  REFIID riid,
                  void** ppv)
{
  initialize_logger();
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
DllCanUnloadNow();

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllCanUnloadNow()
{
  initialize_logger();
  ELLE_TRACE_SCOPE("check if the dll can unload (lock: %s)", g_DllLockCounter);

  return g_DllLockCounter > 0 ? S_FALSE : S_OK;
}

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllUnregisterServer();

extern "C"
__declspec(dllexport)
HRESULT __stdcall
DllUnregisterServer()
{
  initialize_logger();
  ELLE_TRACE_SCOPE("unregister server (lock: %s)", g_DllLockCounter);
  return S_OK;
}
