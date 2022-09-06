// PFRefresh.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "PFRefresh.h"
#include "LongFileNameManager.h"
#include "Macros.h"
#include "tstring.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
TCHAR szAlertBeginMessage[MAX_LOADSTRING];      // クリーンアップ開始の警告メッセージ
TCHAR szCannotGetEnvironmentMessage[MAX_LOADSTRING]; // 環境変数を取得できなかった時のメッセージ
TCHAR szCannotLaunchMessage[MAX_LOADSTRING];    // アプリケーションを起動できなかった時のメッセージ
TCHAR szCompletionMessage[MAX_LOADSTRING];      // 正常終了時のメッセージ
TCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // グローバル文字列を初期化する
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_PFREFRESH, szWindowClass, MAX_LOADSTRING);
    LoadString(hInstance, IDS_ALERTBEGINMESSAGE, szAlertBeginMessage, MAX_LOADSTRING);
    LoadString(hInstance, IDS_CANNOTGETENVIRONMENTMESSAGE, szCannotGetEnvironmentMessage, MAX_LOADSTRING);
    LoadString(hInstance, IDS_CANNOTLAUNCHMESSAGE, szCannotLaunchMessage, MAX_LOADSTRING);
    LoadString(hInstance, IDS_COMPLETIONMESSAGE, szCompletionMessage, MAX_LOADSTRING);

    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PFREFRESH));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PFREFRESH));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_PFREFRESH);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_DISABLED,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        const int nResult = PFRefresh::Main();
        // DestroyWindow を呼び出すと WinMain 関数のメッセージ ループから抜けずに終了してしまうため
        // PostQuitMessage を使用
        PostQuitMessage(nResult);
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

namespace PFRefresh
{
    bool DeleteFiles(const TCHAR* szDirectory, const bool bRoot)
    {
        std::tstring strLongFileName;
        bool bTooLong;
        std::tstring strDirectroyName;

        if (LongFileNameManager::RequireLongFileName(szDirectory, strLongFileName, bTooLong)) {
            if (bTooLong) {
                return false;
            }

            strDirectroyName = strLongFileName;
        }
        else {
            strDirectroyName = szDirectory;
        }

        std::tstring strFileName = strDirectroyName;
        strFileName += _T("\\*.*");
        WIN32_FIND_DATA findFileData;
        HANDLE hFindFile = FindFirstFileEx(strFileName.c_str(), FindExInfoBasic, &findFileData, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);

        if (hFindFile == INVALID_HANDLE_VALUE) {
            return false;
        }

        bool bResult = DeleteFiles(hFindFile, strDirectroyName.c_str(), &findFileData);
        FindClose(hFindFile);

        if (!bRoot && bResult) {
            bResult &= (RemoveDirectory(strDirectroyName.c_str()) != FALSE);
        }

        return bResult;
    }

    bool DeleteFiles(HANDLE hFindFile, const TCHAR* szDirectory, LPWIN32_FIND_DATA lpFindFileData)
    {
        BOOL bFound = TRUE;
        bool bResult = true;

        while (bFound) {
            // カレントもしくは親ディレクトリ
            const bool skip =
                !(_tcsncmp(lpFindFileData->cFileName, _T("."), _tcslen(lpFindFileData->cFileName)) &&
                _tcsncmp(lpFindFileData->cFileName, _T(".."), _tcslen(lpFindFileData->cFileName)));

            if (!skip) {
                std::tstring strFileName = szDirectory;
                strFileName += _T("\\");
                strFileName += lpFindFileData->cFileName;
                std::tstring strLongFileName;
                bool bTooLong;

                if (LongFileNameManager::RequireLongFileName(strFileName.c_str(), strLongFileName, bTooLong)) {
                    if (bTooLong) {
                        return false;
                    }

                    strFileName = strLongFileName;
                }

                if ((lpFindFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
                    // ファイル
                    bResult &= (DeleteFile(strFileName.c_str()) != FALSE);
                }
                else {
                    // ディレクトリ
                    bResult &= DeleteFiles(strFileName.c_str(), false);
                }
            }

            bFound = FindNextFile(hFindFile, lpFindFileData);
        }

        return bResult;
    }

    int EncloseWithDoubleQuotes(TCHAR* szText, const size_t nDestSize)
    {
        const size_t sourceLength = _tcslen(szText);
        const size_t length = (sourceLength + 3);
        TCHAR* szTemp = new TCHAR[length];
        _tcscpy_s(szTemp, length, _T("\""));
        _tcscat_s(szTemp, length, szText);
        _tcscat_s(szTemp, length, _T("\""));
        const int nResult = _tcscpy_s(szText, nDestSize, szTemp);
        delete[] szTemp;
        return nResult;
    }

    DWORD GetEnvironmentVariable(LPCTSTR lpName, LPTSTR lpBuffer, DWORD nSize)
    {
        const DWORD dwResult = ::GetEnvironmentVariable(lpName, lpBuffer, nSize);

        if (!dwResult) {
            ShowErrorMessage(szCannotGetEnvironmentMessage, nullptr);
        }

        return dwResult;
    }

    BOOL LaunchAsAdministrator(LPCTSTR lpFile, LPCTSTR lpParameters, const bool bShowErrorMessage)
    {
        SHELLEXECUTEINFO executeInfo;
        ZeroMemory_(executeInfo);
        executeInfo.cbSize = sizeof(executeInfo);
        executeInfo.fMask = 0;
        executeInfo.hwnd = nullptr;
        executeInfo.lpVerb = _T("runas");
        executeInfo.lpFile = lpFile;
        executeInfo.lpParameters = lpParameters;
        executeInfo.nShow = SW_SHOW;
        const BOOL bResult = ShellExecuteEx(&executeInfo);

        if (!bResult && bShowErrorMessage) {
            ShowErrorMessage(szCannotLaunchMessage, nullptr);
        }

        return bResult;
    }

    int Main()
    {
        int nResult = 0;

        if (__argc < 2) {
            if (PFRefresh::ShowMessage(szAlertBeginMessage, nullptr, (MB_ICONEXCLAMATION | MB_OKCANCEL)) != IDOK) {
                return 0;
            }

            LPCTSTR lpFile = __targv[0];
            TCHAR szTemp[MAX_PATH] = { '\0' };
            const size_t nTempBufferSize = _countof(szTemp);
            TCHAR szParametors[MAX_PATH * 2] = { '\0' };

            if (PFRefresh::GetEnvironmentVariable(_T("TEMP"), szTemp, nTempBufferSize)) {
                PFRefresh::EncloseWithDoubleQuotes(szTemp, nTempBufferSize);
                _tcscat_s(szParametors, szTemp);
                ZeroMemory_(szTemp);
            }

            if (PFRefresh::GetEnvironmentVariable(_T("windir"), szTemp, nTempBufferSize)) {
                if (szParametors[0]) {
                    _tcscat_s(szParametors, _T(" "));
                }

                PathAddBackslash(szTemp);
                _tcscat_s(szTemp, _T("Temp"));
                PFRefresh::EncloseWithDoubleQuotes(szTemp, nTempBufferSize);
                _tcscat_s(szParametors, szTemp);
            }

            nResult = PFRefresh::LaunchAsAdministrator(lpFile, szParametors, true);
        }
        else {
            for (int i = 1; i < __argc; ++i) {
                PFRefresh::DeleteFiles(__targv[i], true);
            }

            if (!nResult) {
                PFRefresh::ShowMessage(szCompletionMessage, nullptr, (MB_ICONINFORMATION | MB_OK));
            }
        }

        return nResult;
    }

    int ShowErrorMessage(LPCTSTR lpText, LPCTSTR lpCaption)
    {
        return ShowMessage(lpText, lpCaption, (MB_ICONERROR | MB_OK));
    }

    int ShowMessage(LPCTSTR lpText, LPCTSTR lpCaption, const UINT uType)
    {
        const LPCTSTR lpCaption_ = (lpCaption ? lpCaption : szTitle);
        // hWnd を渡しても、WS_OVERLAPPEDWINDOW で作成しても MB_SETFOREGROUND ではフォアグラウンドにならないため、MB_TOPMOST を指定
        // (※ウィンドウ ハンドルを作成しないと MB_TOPMOST も効果がない)
        return MessageBox(nullptr, lpText, lpCaption_, (uType | MB_TOPMOST));
    }
}
