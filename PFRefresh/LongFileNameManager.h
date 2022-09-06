#pragma once

#include <tchar.h>
#include "tstring.h"

namespace LongFileNameManager
{
    bool RequireLongFileName(const TCHAR* szFileName, std::tstring& strResult, bool& bTooLong);
}
