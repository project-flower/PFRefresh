#include <Windows.h>
#include "LongFileNameManager.h"

namespace LongFileNameManager
{
    static const TCHAR* PREFIX_LONGFILENAME = _T("\\\\?\\");

    bool RequireLongFileName(const TCHAR* szFileName, std::tstring& strResult, bool& bTooLong)
    {
        const size_t length = _tcslen(szFileName);

        if (length > SHRT_MAX) {
            bTooLong = true;
            return true;
        }

        if (_tcslen(szFileName) < MAX_PATH) {
            bTooLong = false;
            return false;
        }

        if (!_UNICODE) {
            bTooLong = true;
            return true;
        }

        if (_tcsncmp(szFileName, PREFIX_LONGFILENAME, _tcslen(PREFIX_LONGFILENAME)) == 0) {
            // \\?\ ‚ÅŽn‚Ü‚é
            bTooLong = (length <= SHRT_MAX);
        }
        else {
            // \\?\ ‚ÅŽn‚Ü‚ç‚È‚¢
            if ((length + _tcslen(PREFIX_LONGFILENAME)) > SHRT_MAX) {
                bTooLong = true;
                return true;
            }

            bTooLong = false;
            strResult = PREFIX_LONGFILENAME;
            strResult += szFileName;
        }

        return true;
    }
}
