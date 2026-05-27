// Redirect to main WindowsCompat.h in include/ directory
// This file exists for source tree compatibility when include/ is in the include path
// Uses include guard to prevent circular includes
#ifndef _WINDOWS_COMPAT_REDIRECT_H
#define _WINDOWS_COMPAT_REDIRECT_H

// Check if the main WindowsCompat.h has already been included (via its guard)
#ifdef _WINDOWS_COMPAT_H
#include "Common.h"
#else
// Include the actual implementation from include/
#include "../../../include/WindowsCompat.h"
#endif

#endif // _WINDOWS_COMPAT_REDIRECT_H