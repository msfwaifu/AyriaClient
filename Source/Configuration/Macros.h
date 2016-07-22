/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        Preprocessor macros for utilities.
*/

#pragma once
#include <Configuration\All.h>
#include <Utilities\All.h>

// Debug information.
#ifdef NDEBUG
#define PrintFunction()
#define DebugPrint(string)
#define InfoPrint(string)
#else
#define PrintFunction() AppendToLogfile(__FUNCTION__, "Call to")
#define DebugPrint(string) AppendToLogfile(string, "Debug")
#define InfoPrint(string) AppendToLogfile(string, "Info")
#define NetworkPrint(string) AppendToLogfile(string, "Network")
#endif
