/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        Updates the cache as needed.
*/

#include "Cachemanager.h"
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <ctime>

namespace Cachemanager
{
    // Local map of the blocks.
    std::unordered_map<uint32_t /* Identifier */, std::unique_ptr<Cacheblock>> Cachemap;

    // Update and try to sleep for as long as possible.
    void Updatethread()
    {
        while (true)
        {
            uint32_t Timestamp = std::time(NULL);
            uint32_t Nextupdate = Timestamp;

            for (auto Iterator = Cachemap.begin(); Iterator != Cachemap.end(); ++Iterator)
            {
                // Update the cache if expired.
                if (Iterator->second->Expiration <= Timestamp)
                {
                    Iterator->second->Update();
                    Iterator->second->Expiration = Timestamp + Iterator->second->Updaterate;
                }

                // Find how long we can sleep.
                Nextupdate = std::min(Iterator->second->Expiration, Nextupdate);
            }

            // Sleep until the next update or 500ms in case a new block is added.
            std::this_thread::sleep_for(std::chrono::milliseconds(std::min(Nextupdate * 1000, uint32_t(500))));
        }
    }

    // Add the block to the internal queue.
    void Registerblock(Cacheblock *Block)
    {
        Cachemap[Block->Identifier] = std::make_unique<Cacheblock>(*Block);
    }
    void Registerblock(Cacheblock *Block, uint32_t Timeout)
    {
        Block->Updaterate = Timeout;
        Block->Expiration = 0;
        Registerblock(Block);
    }
    void Registerblock(Cacheblock *Block, uint32_t Timeout, uint32_t ID)
    {
        Block->Updaterate = Timeout;
        Block->Identifier = ID;
        Block->Expiration = 0;
        Registerblock(Block);
    }

    // Get a block from the queue.
    std::shared_ptr<char[]> Getblock(uint32_t ID)
    {
        if (Cachemap.find(ID) != Cachemap.end())
            return Cachemap[ID]->Databuffer;
        else
            return nullptr;
    }

    // Remove cache tracking for the block.
    void Removeblock(uint32_t ID)
    {
        if (Cachemap.find(ID) != Cachemap.end())
            Cachemap.erase(ID);
    }

    // Create the thread on startup, hackery.
    struct Startthread { Startthread() { std::thread(Updatethread).detach(); } };
    static Startthread Threadhack;
}