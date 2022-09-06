#pragma once

#include "resource.h"

namespace PFRefresh
{
    bool DeleteFiles(const TCHAR* szDirectory);
    bool DeleteFiles(HANDLE hFindFile, const TCHAR* szDirectory, LPWIN32_FIND_DATA findData);
    int EncloseWithDoubleQuotes(TCHAR* szText, const size_t nDestSize);
    DWORD GetEnvironmentVariable(LPCTSTR lpName, LPTSTR lpBuffer, DWORD nSize);
    BOOL LaunchAsAdministrator(LPCTSTR lpFile, LPCTSTR lpParameters, const bool bShowErrorMessage);
    int Main();
    int ShowErrorMessage(LPCTSTR lpText, LPCTSTR lpCaption);
    int ShowMessage(LPCTSTR lpText, LPCTSTR lpCaption, const UINT uType);
}
