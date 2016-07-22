/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        Routes messages to subsystems.
*/

#pragma once
#include <Configuration\All.h>
#include <cstdarg>

// Subsystem callback on message.
using mCallback = void (*) (va_list Arguments);

namespace Messagehandler
{
    // Register a callback for the subsystem.
    void Registerhandler(uint32_t MessageID, mCallback Handler);

    // Route the message to the subsystem.
    void Handlemessage(uint32_t MessageID, va_list Arguments);

    // Request-synchronization.
    void Addrequestfront();
    void Removerequest();
    void Waitrequest();
    void Addrequest();

    // Create a pipe for alternative communication.
    void Createpipeinterface();
    void Senddataonpipe(std::string &Databuffer);
    void Senddataonpipe(uint32_t Length, const void *Databuffer);
};
