// BemacsMoniker.h : Declaration of the CBemacsMoniker

#ifndef __BEMACSMONIKER_H_
#define __BEMACSMONIKER_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CBemacsMoniker
class ATL_NO_VTABLE CBemacsMoniker : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBemacsMoniker, &CLSID_BemacsMoniker>,
	public IMoniker,
	public IParseDisplayName,
	public IROTData
{
public:
	CBemacsMoniker();
	virtual ~CBemacsMoniker();


	// IPersist
        virtual HRESULT STDMETHODCALLTYPE GetClassID( 
            /* [out] */ CLSID __RPC_FAR *pClassID);

	// IPersistStream
        virtual HRESULT STDMETHODCALLTYPE IsDirty( void);
        
        virtual HRESULT STDMETHODCALLTYPE Load( 
            /* [unique][in] */ IStream __RPC_FAR *pStm);
        
        virtual HRESULT STDMETHODCALLTYPE Save( 
            /* [unique][in] */ IStream __RPC_FAR *pStm,
            /* [in] */ BOOL fClearDirty);
        
        virtual HRESULT STDMETHODCALLTYPE GetSizeMax( 
            /* [out] */ ULARGE_INTEGER __RPC_FAR *pcbSize);

	// IMoniker
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE BindToObject( 
            /* [unique][in] */ IBindCtx __RPC_FAR *pbc,
            /* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
            /* [in] */ REFIID riidResult,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvResult);
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE BindToStorage( 
            /* [unique][in] */ IBindCtx __RPC_FAR *pbc,
            /* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObj);
        
        virtual HRESULT STDMETHODCALLTYPE Reduce( 
            /* [unique][in] */ IBindCtx __RPC_FAR *pbc,
            /* [in] */ DWORD dwReduceHowFar,
            /* [unique][out][in] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkToLeft,
            /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkReduced);
        
        virtual HRESULT STDMETHODCALLTYPE ComposeWith( 
            /* [unique][in] */ IMoniker __RPC_FAR *pmkRight,
            /* [in] */ BOOL fOnlyIfNotGeneric,
            /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkComposite);
        
        virtual HRESULT STDMETHODCALLTYPE Enum( 
            /* [in] */ BOOL fForward,
            /* [out] */ IEnumMoniker __RPC_FAR *__RPC_FAR *ppenumMoniker);
        
        virtual HRESULT STDMETHODCALLTYPE IsEqual( 
            /* [unique][in] */ IMoniker __RPC_FAR *pmkOtherMoniker);
        
        virtual HRESULT STDMETHODCALLTYPE Hash( 
            /* [out] */ DWORD __RPC_FAR *pdwHash);
        
        virtual HRESULT STDMETHODCALLTYPE IsRunning( 
            /* [unique][in] */ IBindCtx __RPC_FAR *pbc,
            /* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
            /* [unique][in] */ IMoniker __RPC_FAR *pmkNewlyRunning);
        
        virtual HRESULT STDMETHODCALLTYPE GetTimeOfLastChange( 
            /* [unique][in] */ IBindCtx __RPC_FAR *pbc,
            /* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
            /* [out] */ FILETIME __RPC_FAR *pFileTime);
        
        virtual HRESULT STDMETHODCALLTYPE Inverse( 
            /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmk);
        
        virtual HRESULT STDMETHODCALLTYPE CommonPrefixWith( 
            /* [unique][in] */ IMoniker __RPC_FAR *pmkOther,
            /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkPrefix);
        
        virtual HRESULT STDMETHODCALLTYPE RelativePathTo( 
            /* [unique][in] */ IMoniker __RPC_FAR *pmkOther,
            /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkRelPath);
        
        virtual HRESULT STDMETHODCALLTYPE GetDisplayName( 
            /* [unique][in] */ IBindCtx __RPC_FAR *pbc,
            /* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
            /* [out] */ LPOLESTR __RPC_FAR *ppszDisplayName);
        
        virtual HRESULT STDMETHODCALLTYPE ParseDisplayName( 
            /* [unique][in] */ IBindCtx __RPC_FAR *pbc,
            /* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
            /* [in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG __RPC_FAR *pchEaten,
            /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkOut);
        
        virtual HRESULT STDMETHODCALLTYPE IsSystemMoniker( 
            /* [out] */ DWORD __RPC_FAR *pdwMksys);

	// IParseDisplayName
        virtual HRESULT STDMETHODCALLTYPE ParseDisplayName( 
            /* [unique][in] */ IBindCtx __RPC_FAR *pbc,
            /* [in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG __RPC_FAR *pchEaten,
            /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkOut);

DECLARE_REGISTRY_RESOURCEID(IDR_BEMACSMONIKER)
DECLARE_NOT_AGGREGATABLE(CBemacsMoniker)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CBemacsMoniker)
	COM_INTERFACE_ENTRY(IMoniker)
	COM_INTERFACE_ENTRY(IParseDisplayName)
	COM_INTERFACE_ENTRY(IROTData)
END_COM_MAP()

private:
	HRESULT CBemacsMoniker::initialiseObject( IUnknown *pUnk );

// IBemacsMoniker
public:
	wchar_t	process_name[128];
// IROTData
	STDMETHOD(GetComparisonData)(BYTE * pbData, ULONG cbMax, ULONG * pcbData);

};

class DebugTrace
	{
public:
	DebugTrace( const wchar_t *prefix )
		: m_prefix( prefix )
		{
		initBuffer();
		}

	~DebugTrace()
		{
		}

	void add( const wchar_t *t )
		{
		wcscat( m_buffer, t );
		}

	void addLast( const wchar_t *t )
		{
		add( t );
		add( L"\n" );

		OutputDebugStringW( m_buffer );

		initBuffer();
		}

private:
	void initBuffer()
		{
		m_buffer[0] = 0;
		add( m_prefix );
		}

	const wchar_t *m_prefix;
	wchar_t m_buffer[4096];
	};

#endif //__BEMACSMONIKER_H_
