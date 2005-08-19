// BemacsMoniker.cpp : Implementation of CBemacsMoniker
#define INITGUID
#include "stdafx.h"
#include "BEmacsClassMoniker.h"
#include "BemacsMoniker.h"

#include <ExternalInclude/BarrysEmacs.h>

/////////////////////////////////////////////////////////////////////////////
// CBemacsMoniker

static wchar_t bemacs_class_name_colon[] = L"bemacs:";

DebugTrace _t( L"BEmacsMoniker: " );

CBemacsMoniker::CBemacsMoniker()
	{
	_t.addLast( L"CBemacsMoniker::CBemacsMoniker() %(version)s" );
	memset( process_name, 0, sizeof( process_name ) );
	}

CBemacsMoniker::~CBemacsMoniker()
	{
	_t.addLast( L"CBemacsMoniker::~CBemacsMoniker()" );
	}

// Methods used by MkParseDisplayName
HRESULT STDMETHODCALLTYPE CBemacsMoniker::ParseDisplayName
	( 
	/* [unique][in] */ IBindCtx __RPC_FAR *pbc,
	/* [in] */ LPOLESTR pszDisplayName,
	/* [out] */ ULONG __RPC_FAR *pchEaten,
	/* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkOut
	)
	{
	_t.add( L"ParseDisplayName " );
	_t.addLast( pszDisplayName ); 

	wchar_t *p = pszDisplayName;

	while( *p )
		if( *p++ == ':' )
			break;

	if( *p == 0 )
		{
		*ppmkOut = NULL;
		return MK_E_SYNTAX;
		}

	wcscpy( process_name, p );

	*pchEaten = wcslen( pszDisplayName );
	*ppmkOut = static_cast<IMoniker*>(this);
	AddRef();

	return S_OK;
	}

// Used my CoGetObject
HRESULT STDMETHODCALLTYPE CBemacsMoniker::BindToObject
	( 
	/* [unique][in] */ IBindCtx __RPC_FAR *pbc,
	/* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
	/* [in] */ REFIID riidResult,
	/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvResult
	)
	{
	_t.add( L"BindToObject " );
	_t.addLast( process_name );

	CComPtr<IUnknown> pIU;
	HRESULT hr;
	CComPtr<IRunningObjectTable> pIROT;

	*ppvResult = NULL;

	hr = pbc->GetRunningObjectTable(&pIROT);

	if(FAILED(hr))
		{
		_t.addLast( L"BindToObject GetRunningObjectTable() failed" );
		return hr;
		}

	// use Local prefix for terminal server support
	HANDLE mutex = CreateMutex( NULL, FALSE, "Local\\bemacs_bind_moniker" );
	if( mutex == NULL )
		// assume that its an old Windows that does not allow the Local prefix
		mutex = CreateMutex( NULL, FALSE, "bemacs_bind_moniker" );

	if( mutex == NULL )
		{
		_t.addLast( L"BindToObject CreateMutex() failed" );
		return E_FAIL;
		}

	// Request ownership of mutex.

	DWORD dwWaitResult = WaitForSingleObject
			( 
			mutex,		// handle to mutex
			60000L		// 60 second time-out interval
			);

	if( dwWaitResult != WAIT_OBJECT_0 )
		{
		if( dwWaitResult == WAIT_TIMEOUT )
			{
			_t.addLast( L"BindToObject WaitForSingleObject() timeout" );
			return RPC_E_TIMEOUT;
			}
		else
			{
			_t.addLast( L"BindToObject WaitForSingleObject() failed" );
			return E_FAIL;
			}
		}

	_t.addLast( L"BindToObject before GetObject()" );
	hr = pIROT->GetObject(this,&pIU);

	if(hr != S_OK)
		{
		// not running
		_t.addLast( L"BindToObject before CoCreateInstance()" );
		hr = pIU.CoCreateInstance( CLSID_CommandLine );

		if(SUCCEEDED(hr))
			{
			_t.addLast( L"BindToObject CoCreateInstance() success" );
			hr = initialiseObject(pIU);
			}
		else
			{
			_t.addLast( L"BindToObject CoCreateInstance() failed" );
			}
		}

	ReleaseMutex( mutex );

	if(SUCCEEDED(hr))
		{
		_t.addLast( L"BindToObject before RegisterObjectBound()" );

		hr = pbc->RegisterObjectBound(pIU);
		if(SUCCEEDED(hr))
			{
			_t.addLast( L"BindToObject RegisterObjectBound() success" );
			hr = pIU->QueryInterface(riidResult,ppvResult);
			if(FAILED(hr))
				{
				_t.addLast( L"BindToObject QueryInterface() failed" );
				pbc->RevokeObjectBound(pIU);
				}
			}
		else
			_t.addLast( L"BindToObject RegisterObjectBound() failed" );

		}

	return hr;
	}

HRESULT CBemacsMoniker::initialiseObject( IUnknown *pUnk )
	{
	CComPtr<IDispatch> pdisp;
	HRESULT hr;

	_t.addLast( L"initialiseObject()" );

	hr = pUnk->QueryInterface( IID_IDispatch, (void **)&pdisp );
	if( FAILED( hr ) )
		{
		_t.addLast( L"initialiseObject QueryInterface() failed" );
		return hr;
		}

	_t.addLast( L"initialiseObject before GetIDsOfNames( SetProcessName )" );
	OLECHAR FAR* szMember = L"SetProcessName";
	DISPID dispid;
	hr = pdisp->GetIDsOfNames
		(
		IID_NULL, &szMember, 1,
		LOCALE_USER_DEFAULT, &dispid
		);
	if( FAILED( hr ) )
		{
		_t.addLast( L"initialiseObject GetIDsOfNames( SetProcessName ) failed" );
		return hr;
		}

	_t.add( L"initialiseObject before Invoke() process name=" );
	_t.addLast( process_name );

	VARIANTARG string_arg[1] =
		{
		{VT_BSTR, 0, 0, 0}
		};
	DISPPARAMS dispParams = {string_arg, NULL, 1, 0};
	string_arg[0].bstrVal = ::SysAllocString( process_name );

	hr = pdisp->Invoke
		(
		dispid,
		IID_NULL,
		LOCALE_USER_DEFAULT,
		DISPATCH_METHOD,
		&dispParams,
		NULL, NULL, NULL
		);
	if( FAILED( hr ) )
		{
		_t.addLast( L"initialiseObject Invoke() failed" );
		return hr;
		}

	_t.addLast( L"initialiseObject() success" );

	return S_OK;
	}


// Methods Used by the ROT
HRESULT STDMETHODCALLTYPE CBemacsMoniker::GetComparisonData(BYTE * pbData, ULONG cbMax, ULONG * pcbData)
	{
	_t.addLast( L"GetComparisonData()" );
	if (pbData == NULL)
		{
		_t.addLast( L"GetComparisonData() failed pbData == NULL" );
		return E_POINTER;
		}
		
	if (pcbData == NULL)
		{
		_t.addLast( L"GetComparisonData() failed pcbData == NULL" );
		return E_POINTER;
		}


	struct compare_data
		{
		GUID guid;
		wchar_t name[128];
		};

	if( cbMax < sizeof( compare_data ) )
		{
		_t.addLast( L"GetComparisonData() failed out of memory" );
		return E_OUTOFMEMORY;
		}

	compare_data *data = reinterpret_cast<compare_data *>( pbData );

	memset( data, 0, sizeof( compare_data ) );

	memcpy( &data->guid, &CLSID_BemacsMoniker, sizeof( GUID ) );

	wcscpy( data->name, process_name );

	*pcbData = sizeof( compare_data );

	_t.addLast( L"GetComparisonData() success" );

	return S_OK;
	}

HRESULT STDMETHODCALLTYPE CBemacsMoniker::Reduce
	( 
	/* [unique][in] */ IBindCtx __RPC_FAR *pbc,
	/* [in] */ DWORD dwReduceHowFar,
	/* [unique][out][in] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkToLeft,
	/* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkReduced
	)
	{
	_t.addLast( L"Reduce()" );

	*ppmkReduced = this;

	(*ppmkReduced)->AddRef();

	_t.addLast( L"Reduce() return MK_S_REDUCED_TO_SELF" );

	return MK_S_REDUCED_TO_SELF;
	}


HRESULT STDMETHODCALLTYPE CBemacsMoniker::GetDisplayName
	( 
	/* [unique][in] */ IBindCtx __RPC_FAR *pbc,
	/* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
	/* [out] */ LPOLESTR __RPC_FAR *ppszDisplayName
	)
	{
	_t.addLast( L"GetDisplayName()" );

	// see if we have a name
	if( process_name[0] != 0 )
		{
		*ppszDisplayName = reinterpret_cast<wchar_t *>( CoTaskMemAlloc( sizeof( process_name ) + sizeof( bemacs_class_name_colon ) - sizeof( wchar_t ) ) );
		if( *ppszDisplayName == NULL )
			{
			_t.addLast( L"GetDisplayName() out of memory" );
			return E_OUTOFMEMORY;
			}

		wcscpy( *ppszDisplayName, bemacs_class_name_colon );
		wcscat( *ppszDisplayName, process_name );

		_t.add( L"GetDisplayName() display name=" );
		_t.addLast( *ppszDisplayName );

		return S_OK;
		}
	
	// no name available
	_t.addLast( L"GetDisplayName() no name available" );

	return E_NOTIMPL;
	}


HRESULT STDMETHODCALLTYPE CBemacsMoniker::IsRunning
	( 
	/* [unique][in] */ IBindCtx __RPC_FAR *pbc,
	/* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
	/* [unique][in] */ IMoniker __RPC_FAR *pmkNewlyRunning
	)
	{
	_t.addLast( L"IsRunning()" );

	IRunningObjectTable *rot = NULL;
	HRESULT hr = GetRunningObjectTable( 0, &rot );
	if( FAILED( hr ) )
		{
		_t.addLast( L"IsRunning() GetRunningObjectTable() failed" );
		return hr;
		}

	IMoniker* pMoniker;
	hr = QueryInterface (IID_IMoniker, (void**) &pMoniker);
	if( SUCCEEDED (hr) )
		{
		hr = rot->IsRunning (pMoniker);
		if( FAILED( hr ) )
			_t.addLast( L"IsRunning() rot->IsRunning() failed" );

		rot->Release();
		}
	else
		_t.addLast( L"IsRunning() QueryInterface( IID_IMoniker ) failed" );

	return hr;
	}

HRESULT STDMETHODCALLTYPE CBemacsMoniker::IsSystemMoniker
	( 
	/* [out] */ DWORD __RPC_FAR *pdwMksys
	)
	{
	_t.addLast( L"IsSystemMoniker() return S_FALSE" );

	*pdwMksys = MKSYS_NONE;
	return S_FALSE;
	}

HRESULT STDMETHODCALLTYPE CBemacsMoniker::GetClassID
	( 
	/* [out] */ CLSID __RPC_FAR *pClassID
	)
	{
	_t.addLast( L"GetClassID()" );

	*pClassID = CLSID_BemacsMoniker;
	return S_OK;
	}


//--------------------------------------------------------------------------------
//
//	NOT Implemented methods
//
//--------------------------------------------------------------------------------

	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::IsDirty
	( 
	)
	{
	_t.addLast( L"IsDirty() return S_FALSE" );
	return S_FALSE;
	}


	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::Load
	( 
	/* [unique][in] */ IStream __RPC_FAR *pStm
	)
	{
	_t.addLast( L"Load() return S_OK" );
	return S_OK;
	}


	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::Save
	( 
	/* [unique][in] */ IStream __RPC_FAR *pStm,
	/* [in] */ BOOL fClearDirty
	)
	{
	_t.addLast( L"Save() return S_OK" );
	return S_OK;
	}


	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::GetSizeMax
	( 
	/* [out] */ ULARGE_INTEGER __RPC_FAR *pcbSize
	)
	{
	_t.addLast( L"GetSizeMax() return S_OK" );

	pcbSize->QuadPart = 0;

	return S_OK;
	}



HRESULT STDMETHODCALLTYPE CBemacsMoniker::BindToStorage
	( 
	/* [unique][in] */ IBindCtx __RPC_FAR *pbc,
	/* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObj
	)
	{
	_t.addLast( L"BindToStorage() return E_NOTIMPL" );
	*ppvObj = NULL;
	return E_NOTIMPL;
	}


	
	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::ComposeWith
	( 
	/* [unique][in] */ IMoniker __RPC_FAR *pmkRight,
	/* [in] */ BOOL fOnlyIfNotGeneric,
	/* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkComposite
	)
	{
	_t.addLast( L"ComposeWith()" );
	if(fOnlyIfNotGeneric)
	{
		*ppmkComposite = NULL;

		_t.addLast( L"ComposeWith() return MK_E_NEEDGENERIC" );
		return MK_E_NEEDGENERIC;
	}

	_t.addLast( L"ComposeWith() return CreateGenericComposite()" );
	return CreateGenericComposite(this,pmkRight,ppmkComposite);
	}


	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::Enum
	( 
	/* [in] */ BOOL fForward,
	/* [out] */ IEnumMoniker __RPC_FAR *__RPC_FAR *ppenumMoniker
	)
	{
	_t.addLast( L"Enum() return E_NOTIMPL" );

	*ppenumMoniker = NULL;
	return E_NOTIMPL;
	}


	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::IsEqual
	( 
	/* [unique][in] */ IMoniker __RPC_FAR *pmkOtherMoniker
	)
	{
	_t.addLast( L"IsEqual()" );
	CComPtr<IROTData> pRot;
	HRESULT hr;

	hr = pmkOtherMoniker->QueryInterface( IID_IROTData, (void **)&pRot );
	if( FAILED( hr ) )
		return hr;

	BYTE other_buffer[1024];
	ULONG other_size;
	hr = pRot->GetComparisonData( other_buffer, sizeof( other_buffer ), &other_size );

	BYTE this_buffer[1024];
	ULONG this_size;
	hr = GetComparisonData( this_buffer, sizeof( this_buffer ), &this_size );

	if( other_size != this_size )
		{
		_t.addLast( L"IsEqual() return S_FALSE" );
		return S_FALSE;
		}

	for( ULONG i=0; i<this_size; i++ )
		if( this_buffer[i] != other_buffer[i] )
			{
			_t.addLast( L"IsEqual() return S_FALSE" );
			return S_FALSE;
			}

	_t.addLast( L"IsEqual() return S_OK" );
	return S_OK;
	}


	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::Hash
	( 
	/* [out] */ DWORD __RPC_FAR *pdwHash
	)
	{
	_t.addLast( L"Hash() return E_NOTIMPL" );
	return E_NOTIMPL;
	}


	

	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::GetTimeOfLastChange
	( 
	/* [unique][in] */ IBindCtx __RPC_FAR *pbc,
	/* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
	/* [out] */ FILETIME __RPC_FAR *pFileTime
	)
	{
	_t.addLast( L"GetTimeOfLastChange()" );

	HRESULT hr;

	if(pmkToLeft != NULL)
	{
		_t.addLast( L"GetTimeOfLastChange() return time of left" );
		return pmkToLeft->GetTimeOfLastChange(pbc,NULL,pFileTime);
	}

	CComPtr<IRunningObjectTable> pIROT;
	hr = pbc->GetRunningObjectTable(&pIROT);

	if(FAILED(hr))
	{
		_t.addLast( L"GetTimeOfLastChange() GetRunningObjectTable() failed" );
		return E_UNEXPECTED;
	}

	hr = pIROT->GetTimeOfLastChange(this,pFileTime);
	if( FAILED( hr ) )
		_t.addLast( L"GetTimeOfLastChange() rot->GetTimeOfLastChange() failed" );

	_t.addLast( L"GetTimeOfLastChange() success" );
	return hr;
	}


	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::Inverse
	( 
	/* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmk
	)
	{
	_t.addLast( L"Inverse() return MK_E_NOINVERSE" );
	*ppmk = NULL;

	return MK_E_NOINVERSE;
	}


	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::CommonPrefixWith
	( 
	/* [unique][in] */ IMoniker __RPC_FAR *pmkOther,
	/* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkPrefix
	)
	{
	_t.addLast( L"CommonPrefixWith()" );
	HRESULT hr;
	
	hr = IsEqual(pmkOther);

	if(hr == S_OK)
		{
		_t.addLast( L"CommonPrefixWith() return MK_S_US" );
		((IMoniker *)this)->AddRef();
		*ppmkPrefix = this;
		hr = MK_S_US;
		}
	else
		{
		hr = MonikerCommonPrefixWith(this,pmkOther,ppmkPrefix);
		if( FAILED(hr) )
			_t.addLast( L"CommonPrefixWith() MonikerCommonPrefixWith() failed" );
		}

	_t.addLast( L"CommonPrefixWith() done" );
	return hr;
	}


	
HRESULT STDMETHODCALLTYPE CBemacsMoniker::RelativePathTo
	( 
	/* [unique][in] */ IMoniker __RPC_FAR *pmkOther,
	/* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkRelPath
	)
	{
	_t.addLast( L"RelativePathTo" );

	HRESULT hr;
	hr = IsEqual(pmkOther);

	if(hr == S_OK)
		{
		_t.addLast( L"RelativePathTo() return S_OK" );
		((IMoniker *)this)->AddRef();
		*ppmkRelPath = this;
		hr = S_OK;
		}
	else
		{
		hr = MonikerRelativePathTo(this,pmkOther,ppmkRelPath,0);
		if( FAILED(hr) )
			_t.addLast( L"RelativePathTo() MonikerRelativePathTo() failed" );
		}

	return hr;
	}


HRESULT STDMETHODCALLTYPE CBemacsMoniker::ParseDisplayName
	( 
	/* [unique][in] */ IBindCtx __RPC_FAR *pbc,
	/* [unique][in] */ IMoniker __RPC_FAR *pmkToLeft,
	/* [in] */ LPOLESTR pszDisplayName,
	/* [out] */ ULONG __RPC_FAR *pchEaten,
	/* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmkOut
	)
	{
	_t.addLast( L"ParseDisplayName() return E_NOTIMPL" );
	return E_NOTIMPL;
	}
