// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__895FC33E_A2FA_45BD_898E_81F79E700E1B__INCLUDED_)
# define AFX_STDAFX_H__895FC33E_A2FA_45BD_898E_81F79E700E1B__INCLUDED_

# if _MSC_VER > 1000
#  pragma once
# endif

// Insert your headers here
# define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

# include <windows.h>
# include <tchar.h>

// Our DLL's HINSTANCE
extern HINSTANCE g_DllInstance;
// The module lock count
extern UINT g_DllLockCounter;

#endif
