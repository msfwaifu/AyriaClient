/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        Updates the cache as needed.
*/

#pragma once
#include <Configuration\All.h>
#include <memory>

// Standard unit for storage.
struct Cacheblock
{
    uint32_t Identifier;
    uint32_t Updaterate;
    uint32_t Expiration;
    uint32_t Buffersize;
    std::shared_ptr<char[]> Databuffer;

    virtual void Update() {};
};

namespace Cachemanager
{
    // Add the block to the internal map.
    void Registerblock(Cacheblock *Block);
    void Registerblock(Cacheblock *Block, uint32_t Timeout);
    void Registerblock(Cacheblock *Block, uint32_t Timeout, uint32_t BlockID);

    // Get a block from the map.
    std::shared_ptr<char[]> Getblockdata(uint32_t BlockID);
    void Getblockdata(uint32_t BlockID, void *Databuffer, uint32_t *Datalength);

    // Remove cache tracking for the block.
    void Removeblock(uint32_t BlockID);
}
