/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        Exposes the messaging system to the host process.
*/

#include <Configuration\All.h>
#include "Messagehandler.h"
#include <cstdarg>

extern "C"
{
    EXPORT_ATTR void __cdecl onMessage(uint32_t MessageID, ...)
    {
        std::va_list Variadic;
        va_start(Variadic, MessageID);

        // Messages are a 32bit FNV1a hash of a string.
        Messagehandler::Handlemessage(MessageID, Variadic);

        va_end(Variadic);
    }
}
