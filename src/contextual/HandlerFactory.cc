#include <contextual/stdafx.h>
#include <contextual/HandlerFactory.hh>
#include <contextual/Handler.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("contextual.Factory");

namespace fist
{
  namespace contextual
  {
    HandlerFactory::HandlerFactory()
      : _refs(1)
    {
      ELLE_TRACE_SCOPE("%s: construction (lock: %s)", *this, g_DllLockCounter);
      InterlockedIncrement(&g_DllLockCounter);
    }

    HandlerFactory::~HandlerFactory()
    {
      ELLE_TRACE_SCOPE("%s: desconstruction (lock: %s)", *this, g_DllLockCounter);
      InterlockedDecrement(&g_DllLockCounter);
    }

    ULONG __stdcall
    HandlerFactory::AddRef()
    {
      ELLE_TRACE_SCOPE("%s: add ref (currently %s)", *this, this->_refs);
      return InterlockedIncrement(&this->_refs);
    }

    ULONG __stdcall
    HandlerFactory::Release()
    {
      ELLE_TRACE_SCOPE("%s: release ref (currently %s)", *this, this->_refs);
      ULONG uRet = InterlockedDecrement(&this->_refs);
      if (this->_refs == 0)
        delete this;
      return uRet;
    }

    IFACEMETHODIMP
    HandlerFactory::QueryInterface(REFIID riid,
                                   void** ppv)
    {
      ELLE_TRACE_SCOPE("%s: query as %s", *this, riid);
      if (IsBadWritePtr(ppv, sizeof(void*)))
        ELLE_ERR("bad pointer")
          return E_POINTER;
      *ppv = nullptr;

      // If the client is requesting an interface we support, set *ppv.
      HRESULT hrRet = S_OK;
      if (InlineIsEqualGUID(riid, IID_IUnknown))
        ELLE_DEBUG("as a IUnknown")
          *ppv = static_cast<IClassFactory*>(this);
      else if (InlineIsEqualGUID(riid, IID_IClassFactory))
        ELLE_DEBUG("as un IClassFactory")
          *ppv = static_cast<IClassFactory*>(this);
      else
        ELLE_DEBUG("no interface")
          hrRet = E_NOINTERFACE;

      if (hrRet == S_OK)
        this->AddRef();

      return hrRet;
    }


    HRESULT __stdcall
    HandlerFactory::CreateInstance(IUnknown* pUnkOuter,
                                   REFIID riid,
                                   void** ppv)
    {
      ELLE_TRACE_SCOPE("%s: create %s instance", *this, riid);
      if (pUnkOuter != nullptr)
        ELLE_WARN("no aggregation")
          return CLASS_E_NOAGGREGATION;
      if (IsBadWritePtr(ppv, sizeof(void*)))
        ELLE_WARN("bad pointer")
          return E_POINTER;
      *ppv = NULL;

      fist::contextual::Handler* handler =
        new (std::nothrow) fist::contextual::Handler();
      if (handler == nullptr)
        return E_OUTOFMEMORY;
      HRESULT hrRet = handler->QueryInterface(riid, ppv);
      handler->Release();
      if (FAILED(hrRet))
        ELLE_WARN("querying interface failed")
          delete handler;
      return hrRet;
    }

    HRESULT __stdcall
    HandlerFactory::LockServer (BOOL fLock)
    {
      ELLE_TRACE_SCOPE("%s: %slock server (lock: %s)",
                       *this, fLock ? "" : "un", g_DllLockCounter);
      fLock
        ? InterlockedIncrement(&g_DllLockCounter)
        : InterlockedDecrement(&g_DllLockCounter);
      return S_OK;
    }
  }
}
