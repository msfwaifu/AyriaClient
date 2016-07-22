/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        Read the length of a message then fill the buffer on read.
*/

#include "Streambuffer.h"
#include <algorithm>

// Returns false if there's more data and moves the pointer there.
bool Streambuffer::Read(uint32_t *Length, void *&Buffer)
{
    // Clear any old data in the buffer if full.
    if (Full)
    {
        if (Internalbuffer) Internalbuffer.release();
        Internallength = 0;
        Full = false;
    }

    // Get the number of bytes to read.
    uint32_t Remaining = 0;
    if (Internallength == 0)
    {
        // Try to read the length from the stream.
        if (*Length >= 4)
        {
            Remaining = *(uint32_t *)Buffer;
        }

        // Sanity checking for large messages.
        if (Remaining > MAX_MESSAGE_SIZE)
        {
            DebugPrint(va("%s: Got a message of length %u, check endians!", __func__, Remaining));
            return false;
        }
    }
    else
    {
        Remaining = (*(uint32_t *)Internalbuffer.get()) - Internallength;
    }

    // If the message is longer than what's available, copy what we can.
    Remaining = std::min(Remaining, *Length);

    // Set the bool if we got the full message.
    Full = (Internallength + Remaining) == *(uint32_t *)Internalbuffer.get();

    // Create a new buffer for the data and replace the old one, discard the length if full.
    auto Newbuffer = std::make_unique<char[]>(Remaining + Internallength - (Full ? sizeof(uint32_t) : 0));
    std::memcpy(Newbuffer.get(), Internalbuffer.get() + (Full ? sizeof(uint32_t) : 0), Internallength);
    std::memcpy(Newbuffer.get() + Internallength - (Full ? sizeof(uint32_t) : 0), Buffer, Remaining);
    Internallength -= (Full ? sizeof(uint32_t) : 0);
    Internalbuffer.swap(Newbuffer);
    Internallength += Remaining;    
    
    // Move the bufferpointer and size.
    Buffer = (char *)Buffer + Remaining;
    *Length -= Remaining;

    // Return if there's more data.
    return *Length > 0;
}
void Streambuffer::Write(uint32_t Length, const void *Buffer)
{
    // Clear any old data in the buffer.
    if (Internalbuffer) Internalbuffer.release();
    Full = false;

    // Create a new buffer for the data.
    Internallength = Length + sizeof(uint32_t);
    Internalbuffer = std::make_unique<char[]>(Internallength);

    // Copy the size and data to the buffer.
    std::memcpy(Internalbuffer.get() + 0, &Length, sizeof(uint32_t));
    std::memcpy(Internalbuffer.get() + sizeof(uint32_t), Buffer, Length);
}
