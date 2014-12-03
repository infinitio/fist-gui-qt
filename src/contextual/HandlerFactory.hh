#ifndef FIST_GUI_QT_CONTEXTUAL_FACTORY_HH
# define FIST_GUI_QT_CONTEXTUAL_FACTORY_HH

# include <unknwn.h>

namespace fist
{
  namespace contextual
  {
    class HandlerFactory:
      public IClassFactory
    {
    public:
      HandlerFactory();

      virtual
      ~HandlerFactory();

      /*---------.
      | IUnknown |
      `---------*/
      ULONG __stdcall
      AddRef() override;

      ULONG __stdcall
      Release() override;

      HRESULT __stdcall
      QueryInterface(REFIID riid, void** ppv) override;

      /*--------------.
      | IClassFactory |
      `--------------*/
      HRESULT __stdcall
      CreateInstance(IUnknown* pUnkOuter,
                     REFIID riid,
                     void** ppv) override;

      HRESULT __stdcall
      LockServer(BOOL fLock) override;

    protected:
      ULONG _refs;
    };
  }
}

#endif
