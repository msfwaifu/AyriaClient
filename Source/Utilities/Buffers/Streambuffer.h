/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        Read the length of a message then fill the buffer on read.
*/

#pragma once
#include <Configuration\All.h>
#include <memory>

struct Streambuffer
{
    std::unique_ptr<char []> Internalbuffer;
    uint32_t Internallength;
    bool Full;

    // Returns false if there's more data and moves the pointer there.
    bool Read(uint32_t *Length, void *&Buffer);
    void Write(uint32_t Length, const void *Buffer);
};
