/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        Exposes the messaging system to the host process.
        However, pipes are the preferred method.
*/

#include <Configuration\All.h>
#include <Cache\Cachemanager.h>
#include "Messagehandler.h"
#include <cstdarg>

extern "C"
{
    // Send the data to a handler.
    EXPORT_ATTR void __cdecl onMessage(uint32_t MessageID, ...)
    {
        std::va_list Variadic;
        va_start(Variadic, MessageID);

        // Messages are a 32bit FNV1a hash of a string.
        Messagehandler::Handlemessage(MessageID, Variadic);

        va_end(Variadic);
    }

    // Get a cache block created for a service, this should be implemented in a header.
    EXPORT_ATTR void __cdecl getResult(uint32_t BlockID, void *Databuffer, uint32_t *Datalength)
    {
        // Request a block, *Datalength == 0 if it fails.
        Cachemanager::Getblockdata(BlockID, Databuffer, Datalength);
    }
}
